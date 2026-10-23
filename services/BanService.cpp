
#include "BanService.h"
#include "../DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QString>

#include <iostream>

using namespace std;

namespace {
QString qs(const string& s)
{
    return QString::fromUtf8(s.c_str());
}

string ss(const QString& s)
{
    QByteArray bytes = s.toUtf8();
    return string(bytes.constData(), bytes.size());
}

bool trangThaiHopLe(const string& s)
{
    return s == "Trống" ||
           s == "Đã đặt" ||
           s == "Đang sử dụng" ||
           s == "Bảo trì" ||
           s == "Trong";
}

string chuanHoaTrangThaiBan(const string& s)
{
    return s == "Trong" ? "Trống" : s;
}

void inLoiSQL(const QString& noiDung, const QSqlQuery& q);

bool kiemTraLichDatDangHoatDong(const string& maBan, bool& coLich)
{
    coLich = false;
    QSqlQuery q(DatabaseManager::database());
    q.prepare("SELECT TOP 1 1 FROM dbo.DatBan WHERE MaBan = ? AND TrangThai IN (N'Đã đặt', N'Đã nhận bàn')");
    q.addBindValue(qs(maBan));
    if (!q.exec())
    {
        inLoiSQL("Khong kiem tra duoc lich dat cua ban", q);
        return false; // Phan biet loi truy van voi truong hop khong co lich.
    }
    coLich = q.next();
    return true;
}

bool trangThaiBanCoTheDat(const string& maBan, const string& trangThaiMoi)
{
    bool coLich = false;
    if (!kiemTraLichDatDangHoatDong(maBan, coLich))
    {
        cerr << "Khong the cap nhat trang thai ban khi chua xac minh duoc lich dat.\n";
        return false;
    }
    if ((trangThaiMoi == "Trống" || trangThaiMoi == "Bảo trì") && coLich)
    {
        cerr << "Khong the chuyen ban sang trang thai nay khi van co lich dat dang hoat dong.\n";
        return false;
    }
    if (trangThaiMoi == "Đã đặt" && !coLich)
    {
        cerr << "Khong the dat trang thai Da dat khi ban khong co lich dat dang hoat dong.\n";
        return false;
    }
    if (trangThaiMoi == "Đang sử dụng")
    {
        QSqlQuery q(DatabaseManager::database());
        q.prepare("SELECT TOP 1 1 FROM dbo.DatBan WHERE MaBan = ? AND TrangThai = N'Đã nhận bàn'");
        q.addBindValue(qs(maBan));
        if (!q.exec())
        {
            inLoiSQL("Khong kiem tra duoc trang thai nhan ban", q);
            return false;
        }
        if (!q.next())
        {
            cerr << "Chi chuyen ban sang Dang su dung khi co lich da nhan ban.\n";
            return false;
        }
    }
    return true;
}

void inLoiSQL(const QString& noiDung, const QSqlQuery& q)
{
    cerr << noiDung.toStdString()
         << ": " << q.lastError().text().toStdString()
         << '\n';
}
}

// Tải danh sách bàn từ SQL Server
static vector<Ban> taiDanhSachBanSQL()
{
    vector<Ban> ds;

    if (!DatabaseManager::isOpen())
        return ds;

    QSqlQuery q(DatabaseManager::database());

    if (!q.exec(
        "SELECT MaBan, TenBan, SoCho, TrangThai, MaKhuVuc "
        "FROM Ban ORDER BY MaBan"))
    {
        inLoiSQL("Loi tai danh sach ban", q);
        return ds;
    }

    while (q.next())
    {
        ds.emplace_back(
            ss(q.value("MaBan").toString()),
            ss(q.value("TenBan").toString()),
            q.value("SoCho").toInt(),
            ss(q.value("TrangThai").toString()),
            ss(q.value("MaKhuVuc").toString())
        );
    }

    return ds;
}

// Them ban
bool BanService::themBan(const Ban& ban)
{
    const string trangThaiBanMoi = chuanHoaTrangThaiBan(ban.getTrangThai());
    if (!ban.hopLe() ||
        ban.getMaBan().empty() ||
        ban.getTenBan().empty() ||
        ban.getSoCho() <= 0 ||
        !trangThaiHopLe(ban.getTrangThai()) ||
        (trangThaiBanMoi != "Trống" && trangThaiBanMoi != "Bảo trì"))
    {
        cout << "Thong tin ban khong hop le!\n";
        return false;
    }

    if (!DatabaseManager::isOpen())
    {
        cout << "SQL Server chua ket noi!\n";
        return false;
    }

    QSqlQuery q(DatabaseManager::database());
    q.prepare(
        "INSERT INTO Ban "
        "(MaBan, TenBan, SoCho, TrangThai, MaKhuVuc) "
        "VALUES (?, ?, ?, ?, ?)"
    );

    q.addBindValue(qs(ban.getMaBan()));
    q.addBindValue(qs(ban.getTenBan()));
    q.addBindValue(ban.getSoCho());
    q.addBindValue(qs(trangThaiBanMoi));
    q.addBindValue(qs(ban.getMaKhuVuc()));

    if (!q.exec())
    {
        inLoiSQL("Khong them duoc ban", q);
        return false;
    }

    cout << "Them ban vao SQL thanh cong!\n";
    return true;
}

// Sua thong tin ban
bool BanService::suaBan(
    const string& maBan,
    const Ban& banMoi)
{
    if (!banMoi.hopLe() ||
        banMoi.getTenBan().empty() ||
        banMoi.getSoCho() <= 0 ||
        !trangThaiHopLe(banMoi.getTrangThai()))
    {
        cout << "Thong tin ban moi khong hop le!\n";
        return false;
    }

    if (!DatabaseManager::isOpen())
    {
        cout << "SQL Server chua ket noi!\n";
        return false;
    }

    const string trangThaiMoi = chuanHoaTrangThaiBan(banMoi.getTrangThai());
    if (!trangThaiBanCoTheDat(maBan, trangThaiMoi))
        return false;

    QSqlQuery q(DatabaseManager::database());
    q.prepare(
        "UPDATE Ban "
        "SET TenBan = ?, SoCho = ?, TrangThai = ?, MaKhuVuc = ? "
        "WHERE MaBan = ?"
    );

    q.addBindValue(qs(banMoi.getTenBan()));
    q.addBindValue(banMoi.getSoCho());
    q.addBindValue(qs(trangThaiMoi));
    q.addBindValue(qs(banMoi.getMaKhuVuc()));
    q.addBindValue(qs(maBan));

    if (!q.exec())
    {
        inLoiSQL("Khong sua duoc ban", q);
        return false;
    }

    if (q.numRowsAffected() == 0)
    {
        cout << "Khong tim thay ban can sua!\n";
        return false;
    }

    cout << "Sua ban trong SQL thanh cong!\n";
    return true;
}

// Xoa ban: SQL tu kiem tra cac rang buoc khoa ngoai
bool BanService::xoaBan(const string& maBan)
{
    if (!DatabaseManager::isOpen())
    {
        cout << "SQL Server chua ket noi!\n";
        return false;
    }

    QSqlQuery q(DatabaseManager::database());
    q.prepare(
        "DELETE FROM Ban "
        "WHERE MaBan = ? AND TrangThai = N'Trống'"
    );
    q.addBindValue(qs(maBan));

    if (!q.exec())
    {
        inLoiSQL("Khong xoa duoc ban (co the ban dang duoc su dung)", q);
        return false;
    }

    if (q.numRowsAffected() == 0)
    {
        cout << "Khong tim thay ban hoac ban khong o trang thai trong!\n";
        return false;
    }

    cout << "Xoa ban trong SQL thanh cong!\n";
    return true;
}

// Tim ban theo ma
const Ban* BanService::timBan(const string& maBan) const
{
    // Bo nho dem de giu nguyen API cu tra ve con tro.
    static Ban ketQua;

    if (!DatabaseManager::isOpen())
        return nullptr;

    QSqlQuery q(DatabaseManager::database());
    q.prepare(
        "SELECT MaBan, TenBan, SoCho, TrangThai, MaKhuVuc "
        "FROM Ban WHERE MaBan = ?"
    );
    q.addBindValue(qs(maBan));

    if (!q.exec())
    {
        inLoiSQL("Loi tim ban", q);
        return nullptr;
    }

    if (!q.next())
        return nullptr;

    ketQua = Ban(
        ss(q.value("MaBan").toString()),
        ss(q.value("TenBan").toString()),
        q.value("SoCho").toInt(),
        ss(q.value("TrangThai").toString()),
        ss(q.value("MaKhuVuc").toString())
    );

    return &ketQua;
}

// Hien thi danh sach ban
void BanService::hienThiDanhSach() const
{
    vector<Ban> ds = taiDanhSachBanSQL();

    if (ds.empty())
    {
        cout << "Danh sach ban rong hoac khong tai duoc tu SQL!\n";
        return;
    }

    cout << "\n========== DANH SACH BAN SQL ==========\n";

    for (const Ban& b : ds)
    {
        cout << "Ma ban: " << b.getMaBan() << '\n'
             << "Ten ban: " << b.getTenBan() << '\n'
             << "So cho: " << b.getSoCho() << '\n'
             << "Trang thai: " << b.getTrangThai() << '\n'
             << "Ma khu vuc: " << b.getMaKhuVuc() << '\n'
             << "-----------------------------------\n";
    }
}

// Tim ban trong theo so cho
void BanService::timBanTrong(int soChoToiThieu) const
{
    timBanTheoKhuVuc(soChoToiThieu, "");
}

// Tim ban trong theo so cho va khu vuc
void BanService::timBanTheoKhuVuc(
    int soChoToiThieu,
    const string& maKhuVuc) const
{
    if (soChoToiThieu <= 0)
    {
        cout << "So cho toi thieu phai lon hon 0!\n";
        return;
    }

    if (!DatabaseManager::isOpen())
    {
        cout << "SQL Server chua ket noi!\n";
        return;
    }

    QString sql =
        "SELECT MaBan, TenBan, SoCho, MaKhuVuc "
        "FROM Ban WHERE TrangThai = N'Trống' AND SoCho >= ?";

    if (!maKhuVuc.empty())
        sql += " AND MaKhuVuc = ?";

    sql += " ORDER BY SoCho, MaBan";

    QSqlQuery q(DatabaseManager::database());
    q.prepare(sql);
    q.addBindValue(soChoToiThieu);

    if (!maKhuVuc.empty())
        q.addBindValue(qs(maKhuVuc));

    if (!q.exec())
    {
        inLoiSQL("Loi tim ban trong", q);
        return;
    }

    bool coBan = false;

    while (q.next())
    {
        cout << "Ma ban: " << ss(q.value("MaBan").toString())
             << " | Ten ban: " << ss(q.value("TenBan").toString())
             << " | So cho: " << q.value("SoCho").toInt()
             << " | Khu vuc: " << ss(q.value("MaKhuVuc").toString())
             << '\n';
        coBan = true;
    }

    if (!coBan)
        cout << "Khong co ban trong phu hop!\n";
}

// Cap nhat trang thai ban
bool BanService::capNhatTrangThai(
    const string& maBan,
    const string& trangThai)
{
    if (!trangThaiHopLe(trangThai))
    {
        cout << "Trang thai ban khong hop le!\n";
        return false;
    }

    if (!DatabaseManager::isOpen())
    {
        cout << "SQL Server chua ket noi!\n";
        return false;
    }

    // Chuan hoa va khong cho cap nhat trang thai lam mau thuan voi lich dat.
    const string tt = chuanHoaTrangThaiBan(trangThai);
    if (!trangThaiBanCoTheDat(maBan, tt))
        return false;

    QSqlQuery q(DatabaseManager::database());
    q.prepare(
        "UPDATE Ban SET TrangThai = ? WHERE MaBan = ?"
    );
    q.addBindValue(qs(tt));
    q.addBindValue(qs(maBan));

    if (!q.exec())
    {
        inLoiSQL("Khong cap nhat duoc trang thai ban", q);
        return false;
    }

    if (q.numRowsAffected() == 0)
    {
        cout << "Khong tim thay ban!\n";
        return false;
    }

    cout << "Cap nhat trang thai ban trong SQL thanh cong!\n";
    return true;
}

// Lay danh sach ban
const vector<Ban>& BanService::getDanhSachBan() const
{
    // Nap lai danh sach tu SQL vao cache de giu tuong thich code cu.
    danhSachBan = taiDanhSachBanSQL();
    return danhSachBan;
}
