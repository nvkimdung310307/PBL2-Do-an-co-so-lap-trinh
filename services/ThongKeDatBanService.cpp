#include "ThongKeDatBanService.h"
#include "../DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QString>
#include <QDate>
#include <iostream>

using namespace std;

namespace
{
void baoLoi(const char* noiDung, const QSqlQuery& q)
{
    cerr << noiDung << ": " << q.lastError().text().toStdString() << '\n';
}

bool ketNoiDuoc()
{
    if (DatabaseManager::isOpen()) return true;
    cerr << "SQL Server chua ket noi; khong the thong ke truc tiep.\n";
    return false;
}
}

// Thong ke tong quan bang truy van tong hop truc tiep tu SQL Server.
void ThongKeDatBanService::thongKeTongQuan(const DatBanService&) const
{
    if (!ketNoiDuoc()) return;

    QSqlQuery q(DatabaseManager::database());
    const QString sql = QString::fromUtf8(
        "SELECT COUNT(*) AS Tong, "
        "SUM(CASE WHEN TrangThai IN (N'Đã hủy',N'Da huy') THEN 1 ELSE 0 END) AS DaHuy, "
        "SUM(CASE WHEN TrangThai=N'Không đến' OR TrangThai=N'KhongDen' THEN 1 ELSE 0 END) AS KhongDen, "
        "SUM(CASE WHEN TrangThai IN (N'Đã nhận bàn',N'Da nhan ban') THEN 1 ELSE 0 END) AS DaDen, "
        "SUM(CASE WHEN TrangThai IN (N'Đã đặt',N'Da dat') THEN 1 ELSE 0 END) AS DangCho, "
        "SUM(CASE WHEN TrangThai IN (N'Hoàn thành',N'Hoan thanh') THEN 1 ELSE 0 END) AS HoanThanh "
        "FROM dbo.DatBan");
    if (!q.exec(sql)) { baoLoi("Thong ke tong quan dat ban that bai", q); return; }
    if (!q.next()) { cerr << "Khong doc duoc ket qua thong ke.\n"; return; }

    const int tong = q.value("Tong").toInt();
    const int daHuy = q.value("DaHuy").toInt();
    const int khongDen = q.value("KhongDen").toInt();
    const int daDen = q.value("DaDen").toInt();
    const int dangCho = q.value("DangCho").toInt();
    const int hoanThanh = q.value("HoanThanh").toInt();

    cout << "\n========== THONG KE DAT BAN (SQL SERVER) ==========\n"
         << "Tong so luot dat: " << tong << '\n'
         << "So luot da huy: " << daHuy << '\n'
         << "So luot khach khong den: " << khongDen << '\n'
         << "So luot khach da den: " << daDen << '\n'
         << "So luot dang cho: " << dangCho << '\n'
         << "So luot hoan thanh: " << hoanThanh << '\n';
    if (tong > 0)
        cout << "Ty le huy: " << daHuy * 100.0 / tong << "%\n"
             << "Ty le khong den: " << khongDen * 100.0 / tong << "%\n";
}

// Chi tiet theo ngay lay truc tiep tu SQL, khong phu thuoc cache trong service.
void ThongKeDatBanService::thongKeTheoNgay(const DatBanService&, const string& ngay) const
{
    if (!ketNoiDuoc()) return;
    const QDate ngayThongKe = QDate::fromString(QString::fromUtf8(ngay.c_str()), Qt::ISODate);
    if (!ngayThongKe.isValid()) {
        cerr << "Ngay thong ke khong hop le. Hay nhap theo dinh dang YYYY-MM-DD.\n";
        return;
    }
    QSqlQuery q(DatabaseManager::database());
    q.prepare(QString::fromUtf8(
        "SELECT MaDatBan, CONVERT(varchar(5), GioDat, 108) AS GioDat, TrangThai, SoNguoi "
        "FROM dbo.DatBan WHERE NgayDat = ? "
        "ORDER BY GioDat, MaDatBan"));
    q.addBindValue(ngayThongKe);
    if (!q.exec()) { baoLoi("Thong ke dat ban theo ngay that bai", q); return; }

    int tong = 0, dangHieuLuc = 0, daHuy = 0, khongDen = 0, hoanThanh = 0, tongKhach = 0;
    cout << "\n========== THONG KE NGAY " << ngay << " (SQL SERVER) ==========\n";
    while (q.next()) {
        ++tong;
        const string ma = q.value("MaDatBan").toString().toUtf8().toStdString();
        const string gio = q.value("GioDat").toString().toUtf8().toStdString();
        const string tt = q.value("TrangThai").toString().toUtf8().toStdString();
        if (tt == "Đã hủy" || tt == "Da huy") ++daHuy;
        else if (tt == "Không đến" || tt == "KhongDen") ++khongDen;
        else if (tt == "Hoàn thành" || tt == "Hoan thanh") ++hoanThanh;
        else if (tt == "Đã đặt" || tt == "Da dat" ||
                 tt == "Đã nhận bàn" || tt == "Da nhan ban") {
            ++dangHieuLuc;
            tongKhach += q.value("SoNguoi").toInt();
        }
        cout << "Ma dat ban: " << ma << "\nGio dat: " << gio
             << "\nTrang thai: " << tt << "\n-----------------------------\n";
    }
    if (tong == 0) { cout << "Khong co lich dat trong ngay nay!\n"; return; }
    cout << "Tong luot dat trong lich su: " << tong
         << "\nLuot dang hieu luc (Da dat/Da nhan ban): " << dangHieuLuc
         << "\nSo luot huy: " << daHuy
         << "\nSo luot khach khong den: " << khongDen
         << "\nSo luot hoan thanh: " << hoanThanh
         << "\nTong so khach du kien (chi luot dang hieu luc): " << tongKhach << '\n';
}

// Gom nhom gio ngay tren SQL Server, chi tra ve cac gio co luot dat.
void ThongKeDatBanService::thongKeKhungGio(const DatBanService&) const
{
    if (!ketNoiDuoc()) return;
    QSqlQuery q(DatabaseManager::database());
    const QString sql = QString::fromUtf8(
        "SELECT DATEPART(HOUR, GioDat) AS Gio, COUNT(*) AS SoLuong "
        "FROM dbo.DatBan WHERE TrangThai NOT IN (N'Đã hủy',N'Da huy',N'Không đến',N'KhongDen',N'Hoàn thành',N'Hoan thanh') "
        "GROUP BY DATEPART(HOUR, GioDat) ORDER BY Gio");
    if (!q.exec(sql)) { baoLoi("Thong ke khung gio that bai", q); return; }

    int gioDongKhach = -1, nhieuNhat = 0;
    cout << "\n========== THONG KE KHUNG GIO (SQL SERVER) ==========\n";
    while (q.next()) {
        const int gio = q.value("Gio").toInt();
        const int soLuong = q.value("SoLuong").toInt();
        const int gioKetThuc = (gio + 1) % 24;
        cout << (gio < 10 ? "0" : "") << gio << ":00 - "
             << (gioKetThuc < 10 ? "0" : "") << gioKetThuc << ":00 : "
             << soLuong << " luot dat\n";
        if (soLuong > nhieuNhat) { nhieuNhat = soLuong; gioDongKhach = gio; }
    }
    if (gioDongKhach < 0) cout << "Chua co du lieu de thong ke!\n";
    else {
        const int gioKetThuc = (gioDongKhach + 1) % 24;
        cout << "Khung gio co nhieu luot dat nhat: "
             << (gioDongKhach < 10 ? "0" : "") << gioDongKhach << ":00 - "
             << (gioKetThuc < 10 ? "0" : "") << gioKetThuc
             << ":00\nSo luot dat: " << nhieuNhat << '\n';
    }
}
