#include "DatBanService.h"
#include "../DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QString>
#include <QDate>
#include <QTime>
#include <iostream>

using namespace std;

namespace
{
QString toQString(const string& value)
{
    return QString::fromUtf8(value.data(), static_cast<int>(value.size()));
}

string toString(const QVariant& value)
{
    const QByteArray bytes = value.toString().toUtf8();
    return string(bytes.constData(), static_cast<size_t>(bytes.size()));
}

bool execute(QSqlQuery& query, const char* action)
{
    if (query.exec())
        return true;

    cerr << action << ": "
         << query.lastError().text().toStdString() << '\n';
    return false;
}

string chuanHoaTrangThai(const string& status)
{
    if (status == "Da dat" || status == "Đã đặt") return "Đã đặt";
    if (status == "Da nhan ban" || status == "Đã nhận bàn" || status == "DaDen") return "Đã nhận bàn";
    if (status == "Da huy" || status == "Đã hủy" || status == "Huy" ||
        status == "Hủy") return "Đã hủy";
    if (status == "KhongDen" || status == "Khong den" || status == "Không đến") return "Không đến";
    if (status == "Hoan thanh" || status == "Hoàn thành") return "Hoàn thành";
    return status;
}

bool duLieuNgayGioHopLe(const DatBan& datBan)
{
    const QDate ngay = QDate::fromString(toQString(datBan.getNgayDat()), Qt::ISODate);
    const QTime gio = QTime::fromString(toQString(datBan.getGioDat()), "HH:mm");
    return ngay.isValid() && gio.isValid();
}

void inDatBan(const DatBan& db)
{
    cout << "Ma dat ban: " << db.getMaDatBan() << '\n'
         << "Ma khach hang: " << db.getMaKH() << '\n'
         << "Ma ban: " << db.getMaBan() << '\n'
         << "Ma nhan vien: " << db.getMaNV() << '\n'
         << "Ngay dat: " << db.getNgayDat() << '\n'
         << "Gio dat: " << db.getGioDat() << '\n'
         << "So nguoi: " << db.getSoNguoi() << '\n'
         << "Trang thai: " << db.getTrangThai() << '\n'
         << "Ghi chu: " << db.getGhiChu() << '\n'
         << "---------------------------------------\n";
}
}

bool DatBanService::napDanhSachSQL() const
{
    danhSachDatBan.clear();
    if (!DatabaseManager::isOpen())
    {
        cerr << "SQL Server chua ket noi; khong the tai danh sach dat ban.\n";
        return false;
    }

    QSqlQuery query(DatabaseManager::database());
    const QString sql =
        "SELECT MaDatBan, MaKH, MaBan, ISNULL(MaNV, ''), "
        "CONVERT(varchar(10), NgayDat, 23), CONVERT(varchar(5), GioDat, 108), "
        "SoNguoi, TrangThai, ISNULL(GhiChu, '') "
        "FROM dbo.DatBan ORDER BY NgayDat DESC, GioDat DESC, MaDatBan";

    if (!execute(query, "Tai danh sach dat ban tu SQL that bai"))
        return false;

    while (query.next())
    {
        danhSachDatBan.emplace_back(
            toString(query.value(0)), toString(query.value(1)),
            toString(query.value(2)), toString(query.value(3)),
            toString(query.value(4)), toString(query.value(5)),
            query.value(6).toInt(), toString(query.value(7)),
            toString(query.value(8)));
    }
    return true;
}

bool DatBanService::themDatBan(const DatBan& datBan)
{
    if (!datBan.hopLe() || !duLieuNgayGioHopLe(datBan))
    {
        cerr << "Thong tin dat ban khong hop le (kiem tra ma, ngay, gio, so nguoi).\n";
        return false;
    }
    if (!DatabaseManager::isOpen())
    {
        cerr << "SQL Server chua ket noi. Khong tao dat ban trong bo nho thay the.\n";
        return false;
    }

    QSqlQuery query(DatabaseManager::database());
    query.prepare("EXEC dbo.sp_DatBan_Tao ?, ?, ?, ?, ?, ?, ?, ?");
    query.addBindValue(toQString(datBan.getMaDatBan()));
    query.addBindValue(toQString(datBan.getMaKH()));
    query.addBindValue(toQString(datBan.getMaBan()));
    query.addBindValue(datBan.getMaNV().empty() ? QVariant() : QVariant(toQString(datBan.getMaNV())));
    query.addBindValue(toQString(datBan.getNgayDat()));
    query.addBindValue(toQString(datBan.getGioDat()));
    query.addBindValue(datBan.getSoNguoi());
    query.addBindValue(datBan.getGhiChu().empty() ? QVariant() : QVariant(toQString(datBan.getGhiChu())));

    if (!execute(query, "Tao dat ban that bai"))
        return false;
    query.finish();

    napDanhSachSQL();
    cout << "Dat ban thanh cong va da luu vao SQL Server.\n";
    return true;
}

bool DatBanService::suaDatBan(const string& maDatBan, const DatBan& datBanMoi)
{
    if (!datBanMoi.hopLe() || !duLieuNgayGioHopLe(datBanMoi))
    {
        cerr << "Thong tin dat ban moi khong hop le.\n";
        return false;
    }
    if (!DatabaseManager::isOpen())
    {
        cerr << "SQL Server chua ket noi.\n";
        return false;
    }

    QSqlQuery query(DatabaseManager::database());
    query.prepare("EXEC dbo.sp_DatBan_Sua ?, ?, ?, ?, ?, ?, ?, ?, ?");
    query.addBindValue(toQString(maDatBan));
    query.addBindValue(toQString(datBanMoi.getMaKH()));
    query.addBindValue(toQString(datBanMoi.getMaBan()));
    query.addBindValue(datBanMoi.getMaNV().empty() ? QVariant() : QVariant(toQString(datBanMoi.getMaNV())));
    query.addBindValue(toQString(datBanMoi.getNgayDat()));
    query.addBindValue(toQString(datBanMoi.getGioDat()));
    query.addBindValue(datBanMoi.getSoNguoi());
    query.addBindValue(toQString(chuanHoaTrangThai(datBanMoi.getTrangThai())));
    query.addBindValue(datBanMoi.getGhiChu().empty() ? QVariant() : QVariant(toQString(datBanMoi.getGhiChu())));

    if (!execute(query, "Sua dat ban that bai"))
        return false;
    query.finish();

    napDanhSachSQL();
    cout << "Sua dat ban thanh cong trong SQL Server.\n";
    return true;
}


bool DatBanService::chuyenBanSQL(const string& maDatBan, const string& maBanMoi)
{
    if (maDatBan.empty() || maBanMoi.empty()) { cerr << "Ma dat ban/ban moi khong hop le.\n"; return false; }
    if (!DatabaseManager::connect()) { cerr << "SQL Server chua ket noi.\n"; return false; }
    QSqlQuery query(DatabaseManager::database());
    query.prepare("EXEC dbo.sp_DatBan_ChuyenBan @MaDatBan=?, @MaBanMoi=?");
    query.addBindValue(toQString(maDatBan)); query.addBindValue(toQString(maBanMoi));
    if (!execute(query, "Chuyen ban that bai")) return false;
    query.finish();
    napDanhSachSQL();
    cout << "Chuyen ban thanh cong va da cap nhat SQL Server.\n";
    return true;
}

bool DatBanService::huyDatBan(const string& maDatBan)
{
    if (!DatabaseManager::isOpen()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("EXEC dbo.sp_DatBan_Huy ?");
    query.addBindValue(toQString(maDatBan));
    if (!execute(query, "Huy dat ban that bai")) return false;
    query.finish();
    napDanhSachSQL();
    cout << "Huy dat ban thanh cong.\n";
    return true;
}

bool DatBanService::xacNhanKhachDen(const string& maDatBan)
{
    if (!DatabaseManager::isOpen()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("EXEC dbo.sp_DatBan_XacNhanDen ?");
    query.addBindValue(toQString(maDatBan));
    if (!execute(query, "Xac nhan khach den that bai")) return false;
    query.finish();
    napDanhSachSQL();
    cout << "Da xac nhan khach den; trang thai ban duoc dong bo.\n";
    return true;
}

bool DatBanService::hoanThanhDatBan(const string& maDatBan)
{
    if (!DatabaseManager::isOpen()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("EXEC dbo.sp_DatBan_HoanThanh ?");
    query.addBindValue(toQString(maDatBan));
    if (!execute(query, "Hoan thanh dat ban that bai")) return false;
    query.finish();
    napDanhSachSQL();
    cout << "Da hoan thanh luot dat; trang thai ban duoc dong bo.\n";
    return true;
}

bool DatBanService::danhDauKhongDen(const string& maDatBan)
{
    if (!DatabaseManager::isOpen()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("EXEC dbo.sp_DatBan_KhongDen ?");
    query.addBindValue(toQString(maDatBan));
    if (!execute(query, "Cap nhat khach khong den that bai")) return false;
    query.finish();
    napDanhSachSQL();
    cout << "Da cap nhat lich dat; trang thai ban duoc dong bo.\n";
    return true;
}

bool DatBanService::xoaDatBan(const string& maDatBan)
{
    if (!DatabaseManager::isOpen()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("EXEC dbo.sp_DatBan_Xoa ?");
    query.addBindValue(toQString(maDatBan));
    if (!execute(query, "Xoa dat ban that bai")) return false;
    query.finish();
    napDanhSachSQL();
    cout << "Xoa dat ban thanh cong.\n";
    return true;
}

const DatBan* DatBanService::timDatBan(const string& maDatBan) const
{
    if (!napDanhSachSQL()) return nullptr;
    for (const DatBan& datBan : danhSachDatBan)
        if (datBan.getMaDatBan() == maDatBan) return &datBan;
    return nullptr;
}

void DatBanService::timTheoKhachHang(const string& maKH) const
{
    if (!napDanhSachSQL()) return;
    bool found = false;
    for (const DatBan& datBan : danhSachDatBan)
    {
        if (datBan.getMaKH() == maKH)
        {
            inDatBan(datBan);
            found = true;
        }
    }
    if (!found) cout << "Khong tim thay lich dat cua khach hang.\n";
}

void DatBanService::timTheoNgay(const string& ngayDat) const
{
    if (!napDanhSachSQL()) return;
    bool found = false;
    for (const DatBan& datBan : danhSachDatBan)
    {
        if (datBan.getNgayDat() == ngayDat)
        {
            inDatBan(datBan);
            found = true;
        }
    }
    if (!found) cout << "Khong co lich dat vao ngay nay.\n";
}

void DatBanService::hienThiDanhSach() const
{
    if (!napDanhSachSQL()) return;
    if (danhSachDatBan.empty())
    {
        cout << "Danh sach dat ban trong.\n";
        return;
    }
    cout << "\n========== DANH SACH DAT BAN (SQL) ==========\n";
    for (const DatBan& datBan : danhSachDatBan) inDatBan(datBan);
}

const vector<DatBan>& DatBanService::getDanhSachDatBan() const
{
    napDanhSachSQL();
    return danhSachDatBan;
}
