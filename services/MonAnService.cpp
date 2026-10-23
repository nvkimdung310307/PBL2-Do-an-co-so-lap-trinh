#include "MonAnService.h"
#include "../DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QString>
#include <iostream>

using namespace std;

namespace {
QString qstr(const string& value) { return QString::fromUtf8(value.data(), static_cast<int>(value.size())); }
string str(const QVariant& value) { return value.toString().toUtf8().toStdString(); }
void printSqlError(const char* action, const QSqlQuery& query) {
    cerr << action << " that bai: " << query.lastError().text().toStdString() << '\n';
}
bool ready() {
    QString error;
    if (DatabaseManager::isOpen()) return true;
    if (DatabaseManager::connect(&error)) return true;
    cerr << "Khong ket noi duoc SQL Server: " << error.toStdString() << '\n';
    return false;
}
bool trangThaiHopLe(const string& s) {
    return s == "Đang bán" || s == "Hết món" || s == "Ngừng bán";
}
}

bool MonAnService::napDanhSachSQL() const
{
    if (!ready()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("SELECT MaMon, TenMon, LoaiMon, DonGia, DonViTinh, MoTa, HinhAnh, TrangThai FROM dbo.MonAn ORDER BY MaMon");
    if (!query.exec()) { printSqlError("Tai danh sach mon an", query); return false; }

    vector<MonAn> loaded;
    while (query.next()) {
        loaded.emplace_back(str(query.value(0)), str(query.value(1)), str(query.value(2)),
                            query.value(3).toDouble(), str(query.value(4)), str(query.value(5)),
                            str(query.value(6)), str(query.value(7)));
    }
    danhSachMonAn.swap(loaded);
    return true;
}

bool MonAnService::themMonAn(const MonAn& mon)
{
    if (!mon.hopLe() || !trangThaiHopLe(mon.getTrangThai())) {
        cerr << "Thong tin mon an khong hop le (kiem tra gia va trang thai)!\n"; return false;
    }
    if (!ready()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("INSERT INTO dbo.MonAn (MaMon, TenMon, LoaiMon, DonGia, DonViTinh, MoTa, HinhAnh, TrangThai) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(qstr(mon.getMaMon()));
    query.addBindValue(qstr(mon.getTenMon()));
    query.addBindValue(mon.getLoaiMon().empty() ? QVariant(QVariant::String) : QVariant(qstr(mon.getLoaiMon())));
    query.addBindValue(mon.getDonGia());
    query.addBindValue(mon.getDonViTinh().empty() ? QVariant(QVariant::String) : QVariant(qstr(mon.getDonViTinh())));
    query.addBindValue(mon.getMoTa().empty() ? QVariant(QVariant::String) : QVariant(qstr(mon.getMoTa())));
    query.addBindValue(mon.getHinhAnh().empty() ? QVariant(QVariant::String) : QVariant(qstr(mon.getHinhAnh())));
    query.addBindValue(qstr(mon.getTrangThai()));
    if (!query.exec()) { printSqlError("Them mon an (kiem tra ma mon trung/gia/trang thai)", query); return false; }
    napDanhSachSQL();
    cout << "Them mon an thanh cong!\n";
    return true;
}

bool MonAnService::suaMonAn(const string& maMon, const MonAn& monMoi)
{
    if (!monMoi.hopLe() || !trangThaiHopLe(monMoi.getTrangThai())) {
        cerr << "Thong tin mon an moi khong hop le!\n"; return false;
    }
    if (!ready()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("UPDATE dbo.MonAn SET TenMon=?, LoaiMon=?, DonGia=?, DonViTinh=?, MoTa=?, HinhAnh=?, TrangThai=? WHERE MaMon=?");
    query.addBindValue(qstr(monMoi.getTenMon()));
    query.addBindValue(monMoi.getLoaiMon().empty() ? QVariant(QVariant::String) : QVariant(qstr(monMoi.getLoaiMon())));
    query.addBindValue(monMoi.getDonGia());
    query.addBindValue(monMoi.getDonViTinh().empty() ? QVariant(QVariant::String) : QVariant(qstr(monMoi.getDonViTinh())));
    query.addBindValue(monMoi.getMoTa().empty() ? QVariant(QVariant::String) : QVariant(qstr(monMoi.getMoTa())));
    query.addBindValue(monMoi.getHinhAnh().empty() ? QVariant(QVariant::String) : QVariant(qstr(monMoi.getHinhAnh())));
    query.addBindValue(qstr(monMoi.getTrangThai()));
    query.addBindValue(qstr(maMon));
    if (!query.exec()) { printSqlError("Sua mon an", query); return false; }
    if (query.numRowsAffected() == 0) { cerr << "Khong tim thay mon an can sua!\n"; return false; }
    napDanhSachSQL();
    cout << "Sua mon an thanh cong!\n";
    return true;
}

bool MonAnService::xoaMonAn(const string& maMon)
{
    // Giữ lịch sử hóa đơn: đánh dấu Ngừng bán thay vì DELETE.
    return capNhatTrangThai(maMon, "Ngừng bán");
}

const MonAn* MonAnService::timMonAn(const string& maMon) const
{
    if (!napDanhSachSQL()) return nullptr;
    for (const auto& mon : danhSachMonAn) if (mon.getMaMon() == maMon) return &mon;
    return nullptr;
}

void MonAnService::timTheoTen(const string& tenMon) const
{
    if (!napDanhSachSQL()) return;
    bool found = false;
    for (const auto& mon : danhSachMonAn) {
        if (mon.getTenMon().find(tenMon) != string::npos) {
            cout << "Ma mon: " << mon.getMaMon() << '\n'
                 << "Ten mon: " << mon.getTenMon() << '\n'
                 << "Loai mon: " << mon.getLoaiMon() << '\n'
                 << "Don gia: " << mon.getDonGia() << '\n'
                 << "Trang thai: " << mon.getTrangThai() << '\n'
                 << "-----------------------------\n";
            found = true;
        }
    }
    if (!found) cout << "Khong tim thay mon an!\n";
}

void MonAnService::hienThiDanhSach() const
{
    if (!napDanhSachSQL()) return;
    if (danhSachMonAn.empty()) { cout << "Danh sach mon an dang rong!\n"; return; }
    cout << "\n========== DANH SACH MON AN ==========\n";
    for (const auto& mon : danhSachMonAn) {
        cout << "Ma mon: " << mon.getMaMon() << '\n'
             << "Ten mon: " << mon.getTenMon() << '\n'
             << "Loai mon: " << mon.getLoaiMon() << '\n'
             << "Don gia: " << mon.getDonGia() << '\n'
             << "Don vi tinh: " << mon.getDonViTinh() << '\n'
             << "Mo ta: " << mon.getMoTa() << '\n'
             << "Hinh anh: " << mon.getHinhAnh() << '\n'
             << "Trang thai: " << mon.getTrangThai() << '\n'
             << "--------------------------------------\n";
    }
}

bool MonAnService::capNhatTrangThai(const string& maMon, const string& trangThai)
{
    if (!trangThaiHopLe(trangThai)) { cerr << "Trang thai mon an khong hop le!\n"; return false; }
    if (!ready()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("UPDATE dbo.MonAn SET TrangThai=? WHERE MaMon=?");
    query.addBindValue(qstr(trangThai));
    query.addBindValue(qstr(maMon));
    if (!query.exec()) { printSqlError("Cap nhat trang thai mon an", query); return false; }
    if (query.numRowsAffected() == 0) { cerr << "Khong tim thay mon an!\n"; return false; }
    napDanhSachSQL();
    cout << "Cap nhat trang thai mon an thanh cong!\n";
    return true;
}

const vector<MonAn>& MonAnService::getDanhSachMonAn() const
{
    napDanhSachSQL();
    return danhSachMonAn;
}
