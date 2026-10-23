#include "KhachChoService.h"
#include "../DatabaseManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QString>
#include <iostream>

namespace {
QString qs(const std::string& s) { return QString::fromUtf8(s.data(), static_cast<int>(s.size())); }
std::string ss(const QVariant& v) { const QByteArray b=v.toString().toUtf8(); return std::string(b.constData(), static_cast<size_t>(b.size())); }
void logError(const char* action, const QSqlQuery& q) {
    std::cerr << action << ": " << q.lastError().text().toStdString() << '\n';
}
bool ensureDb() {
    QString error;
    if (DatabaseManager::connect(&error)) return true;
    std::cerr << "Khong ket noi duoc SQL Server: " << error.toStdString() << '\n';
    return false;
}
bool runStatusProcedure(const char* proc, const std::string& id, const char* action) {
    if (!ensureDb()) return false;
    QSqlQuery q(DatabaseManager::database());
    q.prepare(QStringLiteral("EXEC dbo.%1 @MaKhachCho=?").arg(QString::fromLatin1(proc)));
    q.addBindValue(qs(id));
    if (!q.exec()) { logError(action, q); return false; }
    q.finish();
    return true;
}
}

bool KhachChoService::themKhachCho(const KhachCho& khach)
{
    if (khach.getMaKhachCho().empty() || khach.getTenKhach().empty() ||
        khach.getSoDienThoai().empty() || khach.getSoLuongKhach() <= 0) {
        std::cerr << "Thong tin khach cho khong hop le!\n";
        return false;
    }
    if (!ensureDb()) return false;
    QSqlQuery q(DatabaseManager::database());
    q.prepare("EXEC dbo.sp_ThemKhachCho @MaKhachCho=?, @TenKhach=?, @SoDienThoai=?, @SoLuongKhach=?, @KhuVucMongMuon=?");
    q.addBindValue(qs(khach.getMaKhachCho()));
    q.addBindValue(qs(khach.getTenKhach()));
    q.addBindValue(qs(khach.getSoDienThoai()));
    q.addBindValue(khach.getSoLuongKhach());
    q.addBindValue(khach.getKhuVucMongMuon().empty() ? QVariant() : QVariant(qs(khach.getKhuVucMongMuon())));
    if (!q.exec()) { logError("Them khach cho that bai", q); return false; }
    q.finish();
    std::cout << "Them khach vao danh sach cho SQL thanh cong!\n";
    return true;
}

void KhachChoService::hienThiDanhSach() const
{
    if (!ensureDb()) return;
    QSqlQuery q(DatabaseManager::database());
    if (!q.exec("EXEC dbo.sp_DanhSachKhachCho @ChiDangCho=0")) {
        logError("Tai danh sach khach cho that bai", q); return;
    }
    bool any=false;
    int index=0;
    while (q.next()) {
        any=true;
        std::cout << "\n--- Khach cho thu " << ++index << " ---\n"
                  << "Ma khach cho: " << ss(q.value("MaKhachCho")) << '\n'
                  << "Ten khach: " << ss(q.value("TenKhach")) << '\n'
                  << "So dien thoai: " << ss(q.value("SoDienThoai")) << '\n'
                  << "So luong khach: " << q.value("SoLuongKhach").toInt() << '\n'
                  << "Khu vuc mong muon: " << ss(q.value("KhuVucMongMuon")) << '\n'
                  << "Trang thai: " << ss(q.value("TrangThai")) << '\n';
    }
    if (!any) std::cout << "Danh sach khach cho rong!\n";
}

bool KhachChoService::goiKhach(const std::string& maKhachCho)
{
    if (maKhachCho.empty()) return false;
    if (!runStatusProcedure("sp_GoiKhachCho", maKhachCho, "Goi khach cho that bai")) return false;
    std::cout << "Da goi khach thanh cong!\n";
    return true;
}

bool KhachChoService::huyKhachCho(const std::string& maKhachCho)
{
    if (maKhachCho.empty()) return false;
    if (!runStatusProcedure("sp_HuyKhachCho", maKhachCho, "Huy khach cho that bai")) return false;
    std::cout << "Da huy khach cho!\n";
    return true;
}
