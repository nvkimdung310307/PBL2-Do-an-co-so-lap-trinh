#include "DangNhapService.h"
#include "PasswordUtils.h"
#include "../DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <iostream>

using namespace std;
namespace {
string valueString(const QVariant& value) { return value.toString().toUtf8().toStdString(); }
string normalizedRole(QString role) {
    if (role == QStringLiteral("Admin") || role == QStringLiteral("Quản lý")) return "Admin";
    if (role == QStringLiteral("Nhân viên")) return "NhanVien";
    return role.toUtf8().toStdString();
}
string normalizedStatus(QString status) {
    if (status == QStringLiteral("Hoạt động")) return "HoatDong";
    if (status == QStringLiteral("Bị khóa")) return "BiKhoa";
    return status.toUtf8().toStdString();
}
}

DangNhapService::DangNhapService(TaiKhoanService& service)
    : taiKhoanService(service)
{
}

const TaiKhoan* DangNhapService::dangNhap(const string& tenDangNhap, const string& matKhau)
{
    if (!DatabaseManager::isOpen()) {
        cerr << "SQL Server chua ket noi!\n";
        return nullptr;
    }
    if (tenDangNhap.empty() || matKhau.empty()) {
        cout << "Vui long nhap ten dang nhap va mat khau!\n";
        return nullptr;
    }

    QSqlQuery query(DatabaseManager::database());
    query.prepare(
        "SELECT TK.MaTK, TK.TenDangNhap, TK.MatKhau, TK.VaiTro, TK.TrangThai, TK.MaNV "
        "FROM TaiKhoan TK INNER JOIN NhanVien NV ON NV.MaNV = TK.MaNV "
        "WHERE TK.TenDangNhap = ?"
    );
    query.addBindValue(QString::fromUtf8(tenDangNhap.c_str()));
    if (!query.exec()) {
        cerr << "Loi truy van dang nhap SQL: " << query.lastError().text().toStdString() << '\n';
        return nullptr;
    }
    if (!query.next()) {
        query.finish();
        cout << "Sai ten dang nhap, mat khau hoac tai khoan khong hoat dong!\n";
        return nullptr;
    }

    const QString maTK = query.value("MaTK").toString();
    const QString storedPassword = query.value("MatKhau").toString();
    const QString role = query.value("VaiTro").toString();
    const QString status = query.value("TrangThai").toString();
    const QString tenTK = query.value("TenDangNhap").toString();
    const QString maNV = query.value("MaNV").toString();
    query.finish();

    if (status != QStringLiteral("Hoạt động")) {
        cout << "Tai khoan da bi khoa hoac khong hoat dong!\n";
        return nullptr;
    }
    if (!PasswordUtils::verifyPassword(QString::fromUtf8(matKhau.c_str()), storedPassword)) {
        cout << "Sai ten dang nhap hoac mat khau!\n";
        return nullptr;
    }

    // Tu dong nang cap mat khau cu dang plaintext sang PBKDF2 sau dang nhap thanh cong.
    if (!PasswordUtils::isPasswordHash(storedPassword)) {
        QSqlQuery upgrade(DatabaseManager::database());
        upgrade.prepare("UPDATE TaiKhoan SET MatKhau = ? WHERE MaTK = ? AND MatKhau = ?");
        upgrade.addBindValue(PasswordUtils::hashPassword(QString::fromUtf8(matKhau.c_str())));
        upgrade.addBindValue(maTK);
        upgrade.addBindValue(storedPassword);
        if (!upgrade.exec()) {
            cerr << "Canh bao: dang nhap duoc nhung chua nang cap duoc mat khau cu: "
                 << upgrade.lastError().text().toStdString() << '\n';
        }
    }

    taiKhoanDangNhap = TaiKhoan(
        valueString(maTK), valueString(tenTK), "", normalizedRole(role),
        valueString(maNV), normalizedStatus(status));
    cout << "Dang nhap SQL thanh cong!\n";
    return &taiKhoanDangNhap;
}
