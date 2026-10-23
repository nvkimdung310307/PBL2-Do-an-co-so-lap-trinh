#include "KhachHangService.h"
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
}

bool KhachHangService::napDanhSachSQL() const
{
    if (!ready()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("SELECT MaKH, HoTen, SoDienThoai, Email, DiaChi FROM dbo.KhachHang ORDER BY MaKH");
    if (!query.exec()) { printSqlError("Tai danh sach khach hang", query); return false; }

    vector<KhachHang> loaded;
    while (query.next()) {
        loaded.emplace_back(str(query.value(0)), str(query.value(1)), str(query.value(2)),
                            str(query.value(3)), str(query.value(4)));
    }
    danhSachKhachHang.swap(loaded);
    return true;
}

bool KhachHangService::themKhachHang(const KhachHang& kh)
{
    if (!kh.hopLe()) { cerr << "Thong tin khach hang khong hop le!\n"; return false; }
    if (!ready()) return false;

    QSqlQuery query(DatabaseManager::database());
    query.prepare("INSERT INTO dbo.KhachHang (MaKH, HoTen, SoDienThoai, Email, DiaChi) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(qstr(kh.getMaKH()));
    query.addBindValue(qstr(kh.getHoTen()));
    query.addBindValue(qstr(kh.getSoDienThoai()));
    query.addBindValue(kh.getEmail().empty() ? QVariant(QVariant::String) : QVariant(qstr(kh.getEmail())));
    query.addBindValue(kh.getDiaChi().empty() ? QVariant(QVariant::String) : QVariant(qstr(kh.getDiaChi())));
    if (!query.exec()) { printSqlError("Them khach hang (kiem tra ma va so dien thoai trung)", query); return false; }
    napDanhSachSQL();
    cout << "Them khach hang thanh cong!\n";
    return true;
}

bool KhachHangService::suaKhachHang(const string& maKH, const KhachHang& khMoi)
{
    if (!khMoi.hopLe()) { cerr << "Thong tin khach hang moi khong hop le!\n"; return false; }
    if (!ready()) return false;

    QSqlQuery query(DatabaseManager::database());
    query.prepare("UPDATE dbo.KhachHang SET HoTen=?, SoDienThoai=?, Email=?, DiaChi=? WHERE MaKH=?");
    query.addBindValue(qstr(khMoi.getHoTen()));
    query.addBindValue(qstr(khMoi.getSoDienThoai()));
    query.addBindValue(khMoi.getEmail().empty() ? QVariant(QVariant::String) : QVariant(qstr(khMoi.getEmail())));
    query.addBindValue(khMoi.getDiaChi().empty() ? QVariant(QVariant::String) : QVariant(qstr(khMoi.getDiaChi())));
    query.addBindValue(qstr(maKH));
    if (!query.exec()) { printSqlError("Sua khach hang (kiem tra so dien thoai trung)", query); return false; }
    if (query.numRowsAffected() == 0) { cerr << "Khong tim thay khach hang can sua!\n"; return false; }
    napDanhSachSQL();
    cout << "Sua khach hang thanh cong!\n";
    return true;
}

bool KhachHangService::xoaKhachHang(const string& maKH)
{
    if (!ready()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("DELETE FROM dbo.KhachHang WHERE MaKH=?");
    query.addBindValue(qstr(maKH));
    if (!query.exec()) {
        cerr << "Xoa khach hang that bai. Khach hang co the dang duoc su dung trong dat ban/hoa don. Chi tiet: "
             << query.lastError().text().toStdString() << '\n';
        return false;
    }
    if (query.numRowsAffected() == 0) { cerr << "Khong tim thay khach hang can xoa!\n"; return false; }
    napDanhSachSQL();
    cout << "Xoa khach hang thanh cong!\n";
    return true;
}

const KhachHang* KhachHangService::timKhachHang(const string& maKH) const
{
    if (!napDanhSachSQL()) return nullptr;
    for (const auto& kh : danhSachKhachHang) if (kh.getMaKH() == maKH) return &kh;
    return nullptr;
}

const KhachHang* KhachHangService::timTheoSoDienThoai(const string& soDienThoai) const
{
    if (!napDanhSachSQL()) return nullptr;
    for (const auto& kh : danhSachKhachHang) if (kh.getSoDienThoai() == soDienThoai) return &kh;
    return nullptr;
}

void KhachHangService::hienThiDanhSach() const
{
    if (!napDanhSachSQL()) return;
    if (danhSachKhachHang.empty()) { cout << "Danh sach khach hang dang rong!\n"; return; }
    cout << "\n======= DANH SACH KHACH HANG =======\n";
    for (const auto& kh : danhSachKhachHang) {
        cout << "Ma KH: " << kh.getMaKH() << '\n'
             << "Ho ten: " << kh.getHoTen() << '\n'
             << "So dien thoai: " << kh.getSoDienThoai() << '\n'
             << "Email: " << kh.getEmail() << '\n'
             << "Dia chi: " << kh.getDiaChi() << '\n'
             << "------------------------------------\n";
    }
}

const vector<KhachHang>& KhachHangService::getDanhSachKhachHang() const
{
    napDanhSachSQL();
    return danhSachKhachHang;
}
