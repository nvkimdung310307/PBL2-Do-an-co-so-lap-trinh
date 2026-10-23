#include "GopBanService.h"
#include "../oop/Ban.h"
#include "../DatabaseManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QUuid>
#include <iostream>
#include <set>

namespace {
QString qs(const std::string& s) { return QString::fromUtf8(s.data(), static_cast<int>(s.size())); }
void logSql(const char* action, const QSqlQuery& q) { std::cerr << action << ": " << q.lastError().text().toStdString() << '\n'; }
}
GopBanService::GopBanService(BanService& ban) : banService(ban) {}

bool GopBanService::gopBan(const vector<string>& danhSachMaBan, const string& maBanDich)
{
    if (danhSachMaBan.size() < 2) { std::cerr << "Phai chon it nhat 2 ban de gop!\n"; return false; }
    std::set<std::string> ids(danhSachMaBan.begin(), danhSachMaBan.end());
    if (ids.size()!=danhSachMaBan.size()) { std::cerr << "Danh sach co ma ban bi trung!\n"; return false; }
    if (ids.find(maBanDich)==ids.end()) { std::cerr << "Ban dich phai nam trong danh sach ban gop!\n"; return false; }
    if (!DatabaseManager::connect()) { std::cerr << "SQL Server chua ket noi.\n"; return false; }
    const auto& cache=banService.getDanhSachBan();
    for (const auto& id: ids) {
        bool found=false;
        for (const Ban& b: cache) if (b.getMaBan()==id) {
            found=true;
            if (b.getTrangThai()!="Đang sử dụng" && b.getTrangThai()!="Dang su dung") {
                std::cerr << "Ban " << id << " phai dang su dung moi duoc gop.\n"; return false;
            }
            break;
        }
        if (!found) { std::cerr << "Khong tim thay ban: " << id << '\n'; return false; }
    }
    QSqlDatabase db=DatabaseManager::database();
    if (!db.transaction()) { std::cerr << "Khong bat dau duoc transaction SQL.\n"; return false; }
    const std::string groupId=QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    QSqlQuery q(db);
    q.prepare("INSERT dbo.NhomBan(MaNhomBan,TrangThai) VALUES(?, 'DangGop')");
    q.addBindValue(qs(groupId));
    if (!q.exec()) { logSql("Tao nhom ban that bai",q); db.rollback(); return false; }
    for (const auto& id: ids) {
        QSqlQuery lock(db);
        lock.prepare("SELECT TrangThai FROM dbo.Ban WITH (UPDLOCK,HOLDLOCK) WHERE MaBan=?");
        lock.addBindValue(qs(id));
        if (!lock.exec() || !lock.next()) { logSql("Kiem tra ban trong SQL that bai",lock); db.rollback(); return false; }
        const QString status=lock.value(0).toString();
        lock.finish();
        if (status!=QString::fromUtf8("Đang sử dụng")) {
            std::cerr << "Trang thai ban da thay doi; huy gop ban.\n"; db.rollback(); return false;
        }
        QSqlQuery active(db);
        active.prepare("SELECT TOP 1 1 FROM dbo.ChiTietNhomBan ct JOIN dbo.NhomBan n ON n.MaNhomBan=ct.MaNhomBan WHERE ct.MaBan=? AND n.TrangThai='DangGop'");
        active.addBindValue(qs(id));
        if (!active.exec()) { logSql("Kiem tra nhom ban hien tai that bai",active); db.rollback(); return false; }
        const bool alreadyGrouped = active.next();
        active.finish();
        if (alreadyGrouped) { std::cerr << "Ban " << id << " dang thuoc mot nhom ban khac.\n"; db.rollback(); return false; }
        QSqlQuery ins(db);
        ins.prepare("INSERT dbo.ChiTietNhomBan(MaNhomBan,MaBan) VALUES(?,?)");
        ins.addBindValue(qs(groupId)); ins.addBindValue(qs(id));
        if (!ins.exec()) { logSql("Luu chi tiet nhom ban that bai",ins); db.rollback(); return false; }
    }
    if (!db.commit()) { std::cerr << "Commit gop ban that bai: " << db.lastError().text().toStdString() << '\n'; db.rollback(); return false; }
    std::cout << "Gop ban thanh cong va da luu nhom ban vao SQL. Ban dich: " << maBanDich << '\n';
    return true;
}
