#include "TaiKhoanService.h"
#include "PasswordUtils.h"
#include "../DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QString>
#include <iostream>

using namespace std;
namespace {
QString qstr(const string& value) { return QString::fromUtf8(value.c_str()); }
string str(const QVariant& value) { return value.toString().toUtf8().toStdString(); }
QString sqlRole(const string& role) {
    if (role == "Admin" || role == "Quản lý") return QStringLiteral("Admin");
    if (role == "NhanVien" || role == "Nhân viên") return QStringLiteral("Nhân viên");
    return QString();
}
string appRole(QString role) {
    if (role == QStringLiteral("Quản lý") || role == QStringLiteral("Admin")) return "Admin";
    if (role == QStringLiteral("Nhân viên") || role == QStringLiteral("NhanVien")) return "NhanVien";
    return role.toUtf8().toStdString();
}
QString sqlStatus(const string& status) {
    if (status == "HoatDong" || status == "Hoạt động") return QStringLiteral("Hoạt động");
    if (status == "BiKhoa" || status == "Bị khóa" || status == "Bi khoa") return QStringLiteral("Bị khóa");
    return QString();
}
string appStatus(QString status) {
    if (status == QStringLiteral("Hoạt động")) return "HoatDong";
    if (status == QStringLiteral("Bị khóa")) return "BiKhoa";
    return status.toUtf8().toStdString();
}
bool sqlReady() {
    if (!DatabaseManager::isOpen()) cerr << "SQL Server chua ket noi!\n";
    return DatabaseManager::isOpen();
}
void reportSqlError(const char* operation, const QSqlQuery& query) {
    cerr << operation << ": " << query.lastError().text().toStdString() << '\n';
}
}

bool TaiKhoanService::napDanhSachSQL() const
{
    if (!sqlReady()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("SELECT MaTK, TenDangNhap, MatKhau, VaiTro, MaNV, TrangThai FROM TaiKhoan ORDER BY MaTK");
    if (!query.exec()) { reportSqlError("Loi tai danh sach tai khoan", query); return false; }

    vector<TaiKhoan> fresh;
    while (query.next()) {
        fresh.emplace_back(str(query.value("MaTK")),
                           str(query.value("TenDangNhap")),
                           str(query.value("MatKhau")),
                           appRole(query.value("VaiTro").toString()),
                           str(query.value("MaNV")),
                           appStatus(query.value("TrangThai").toString()));
    }
    query.finish();
    danhSachTaiKhoan.swap(fresh);
    return true;
}

bool TaiKhoanService::taiKhoanTonTai(const string& maTK) const
{
    if (!sqlReady()) return false;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("SELECT 1 FROM TaiKhoan WHERE MaTK = ?");
    query.addBindValue(qstr(maTK));
    if (!query.exec()) { reportSqlError("Loi kiem tra tai khoan", query); return false; }
    const bool found = query.next();
    query.finish();
    return found;
}

bool TaiKhoanService::themTaiKhoan(const TaiKhoan& tk)
{
    if (!tk.hopLe() || tk.getMatKhau().size() < 10) {
        cout << "Tai khoan khong hop le; mat khau moi phai co it nhat 10 ky tu!\n";
        return false;
    }
    const QString role = sqlRole(tk.getVaiTro());
    const QString status = sqlStatus(tk.getTrangThai().empty() ? "HoatDong" : tk.getTrangThai());
    if (role.isEmpty() || status.isEmpty()) {
        cout << "Vai tro hoac trang thai tai khoan khong hop le!\n";
        return false;
    }
    if (!sqlReady()) return false;

    QSqlQuery query(DatabaseManager::database());
    query.prepare("INSERT INTO TaiKhoan (MaTK, TenDangNhap, MatKhau, VaiTro, MaNV, TrangThai) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(qstr(tk.getMaTK()));
    query.addBindValue(qstr(tk.getTenDangNhap()));
    query.addBindValue(PasswordUtils::hashPassword(qstr(tk.getMatKhau())));
    query.addBindValue(role);
    query.addBindValue(qstr(tk.getMaNV()));
    query.addBindValue(status);
    if (!query.exec()) { reportSqlError("Them tai khoan that bai", query); return false; }
    napDanhSachSQL();
    cout << "Them tai khoan vao SQL thanh cong!\n";
    return true;
}

bool TaiKhoanService::xoaTaiKhoan(const string& maTK)
{
    if (!sqlReady()) return false;
    QSqlDatabase db = DatabaseManager::database();
    if (!db.transaction()) {
        cerr << "Khong bat dau duoc transaction: " << db.lastError().text().toStdString() << '\n';
        return false;
    }

    QSqlQuery check(db);
    check.prepare("SELECT VaiTro, TrangThai FROM dbo.TaiKhoan WITH (UPDLOCK, HOLDLOCK) WHERE MaTK = ?");
    check.addBindValue(qstr(maTK));
    if (!check.exec() || !check.next()) {
        reportSqlError("Kiem tra tai khoan truoc khi xoa that bai", check);
        check.finish(); db.rollback();
        cout << "Khong tim thay tai khoan!\n";
        return false;
    }
    const QString role = check.value(0).toString();
    const QString status = check.value(1).toString();
    const bool isActiveAdmin =
        (role == QStringLiteral("Admin") || role == QStringLiteral("Quản lý")) &&
        status == QStringLiteral("Hoạt động");
    check.finish();

    if (isActiveAdmin) {
        QSqlQuery count(db);
        count.prepare("SELECT COUNT(*) FROM dbo.TaiKhoan WITH (UPDLOCK, HOLDLOCK) "
                      "WHERE VaiTro IN (N'Admin', N'Quản lý') "
                      "AND TrangThai = N'Hoạt động' AND MaTK <> ?");
        count.addBindValue(qstr(maTK));
        if (!count.exec() || !count.next()) {
            reportSqlError("Kiem tra Admin hoat dong con lai that bai", count);
            count.finish(); db.rollback();
            return false;
        }
        const int remainingAdmins = count.value(0).toInt();
        count.finish();
        if (remainingAdmins == 0) {
            db.rollback();
            cout << "Khong the xoa Admin hoat dong cuoi cung!\n";
            return false;
        }
    }

    QSqlQuery del(db);
    del.prepare("DELETE FROM dbo.TaiKhoan WHERE MaTK = ?");
    del.addBindValue(qstr(maTK));
    if (!del.exec() || del.numRowsAffected() != 1) {
        reportSqlError("Xoa tai khoan that bai", del);
        db.rollback();
        return false;
    }
    if (!db.commit()) {
        cerr << "Commit xoa tai khoan that bai: " << db.lastError().text().toStdString() << '\n';
        db.rollback();
        return false;
    }
    napDanhSachSQL();
    cout << "Xoa tai khoan SQL thanh cong!\n";
    return true;
}

bool TaiKhoanService::doiMatKhau(const string& maTK, const string& matKhauCu, const string& matKhauMoi)
{
    if (matKhauMoi.size() < 10) { cout << "Mat khau moi phai co it nhat 10 ky tu!\n"; return false; }
    if (!sqlReady()) return false;
    QSqlQuery read(DatabaseManager::database());
    read.prepare("SELECT MatKhau FROM TaiKhoan WHERE MaTK = ? AND TrangThai = N'Hoạt động'");
    read.addBindValue(qstr(maTK));
    if (!read.exec() || !read.next()) { cout << "Khong tim thay tai khoan dang hoat dong!\n"; return false; }
    const QString oldStored = read.value(0).toString();
    read.finish();
    if (!PasswordUtils::verifyPassword(qstr(matKhauCu), oldStored)) { cout << "Mat khau cu khong dung!\n"; return false; }

    QSqlQuery update(DatabaseManager::database());
    update.prepare("UPDATE TaiKhoan SET MatKhau = ? WHERE MaTK = ?");
    update.addBindValue(PasswordUtils::hashPassword(qstr(matKhauMoi)));
    update.addBindValue(qstr(maTK));
    if (!update.exec() || update.numRowsAffected() != 1) { reportSqlError("Doi mat khau that bai", update); return false; }
    napDanhSachSQL();
    cout << "Doi mat khau SQL thanh cong!\n";
    return true;
}

bool TaiKhoanService::capNhatVaiTro(const string& maTK, const string& vaiTroMoi)
{
    const QString roleMoi = sqlRole(vaiTroMoi);
    if (roleMoi.isEmpty()) {
        cout << "Vai tro chi duoc la Admin hoac NhanVien!\n";
        return false;
    }
    if (!sqlReady()) return false;

    QSqlDatabase db = DatabaseManager::database();
    if (!db.transaction()) {
        cerr << "Khong bat dau duoc transaction: " << db.lastError().text().toStdString() << '\n';
        return false;
    }

    QSqlQuery check(db);
    check.prepare("SELECT VaiTro, TrangThai FROM dbo.TaiKhoan WITH (UPDLOCK, HOLDLOCK) WHERE MaTK = ?");
    check.addBindValue(qstr(maTK));
    if (!check.exec() || !check.next()) {
        reportSqlError("Kiem tra tai khoan truoc khi doi vai tro that bai", check);
        check.finish(); db.rollback();
        cout << "Khong tim thay tai khoan!\n";
        return false;
    }
    const QString roleCu = check.value(0).toString();
    const QString statusCu = check.value(1).toString();
    const bool dangLaAdminHoatDong =
        (roleCu == QStringLiteral("Admin") || roleCu == QStringLiteral("Quản lý")) &&
        statusCu == QStringLiteral("Hoạt động");
    check.finish();

    const bool roleMoiLaAdmin = roleMoi == QStringLiteral("Admin");
    if (dangLaAdminHoatDong && !roleMoiLaAdmin) {
        QSqlQuery count(db);
        count.prepare("SELECT COUNT(*) FROM dbo.TaiKhoan WITH (UPDLOCK, HOLDLOCK) "
                      "WHERE VaiTro IN (N'Admin', N'Quản lý') "
                      "AND TrangThai = N'Hoạt động' AND MaTK <> ?");
        count.addBindValue(qstr(maTK));
        if (!count.exec() || !count.next()) {
            reportSqlError("Kiem tra Admin hoat dong con lai that bai", count);
            count.finish(); db.rollback();
            return false;
        }
        const int remainingAdmins = count.value(0).toInt();
        count.finish();
        if (remainingAdmins == 0) {
            db.rollback();
            cout << "Khong the ha quyen Admin hoat dong cuoi cung!\n";
            return false;
        }
    }

    QSqlQuery update(db);
    update.prepare("UPDATE dbo.TaiKhoan SET VaiTro = ? WHERE MaTK = ?");
    update.addBindValue(roleMoi);
    update.addBindValue(qstr(maTK));
    if (!update.exec() || update.numRowsAffected() != 1) {
        reportSqlError("Cap nhat vai tro that bai", update);
        db.rollback();
        return false;
    }
    if (!db.commit()) {
        cerr << "Commit cap nhat vai tro that bai: " << db.lastError().text().toStdString() << '\n';
        db.rollback();
        return false;
    }
    napDanhSachSQL();
    cout << "Cap nhat vai tro SQL thanh cong!\n";
    return true;
}

bool TaiKhoanService::capNhatTrangThai(const string& maTK, const string& trangThaiMoi)
{
    const QString statusMoi = sqlStatus(trangThaiMoi);
    if (statusMoi.isEmpty()) {
        cout << "Trang thai chi duoc la HoatDong hoac BiKhoa!\n";
        return false;
    }
    if (!sqlReady()) return false;

    QSqlDatabase db = DatabaseManager::database();
    if (!db.transaction()) {
        cerr << "Khong bat dau duoc transaction: " << db.lastError().text().toStdString() << '\n';
        return false;
    }

    QSqlQuery check(db);
    check.prepare("SELECT VaiTro, TrangThai FROM dbo.TaiKhoan WITH (UPDLOCK, HOLDLOCK) WHERE MaTK = ?");
    check.addBindValue(qstr(maTK));
    if (!check.exec() || !check.next()) {
        reportSqlError("Kiem tra tai khoan truoc khi doi trang thai that bai", check);
        check.finish(); db.rollback();
        cout << "Khong tim thay tai khoan!\n";
        return false;
    }
    const QString roleCu = check.value(0).toString();
    const QString statusCu = check.value(1).toString();
    const bool dangLaAdminHoatDong =
        (roleCu == QStringLiteral("Admin") || roleCu == QStringLiteral("Quản lý")) &&
        statusCu == QStringLiteral("Hoạt động");
    check.finish();

    if (dangLaAdminHoatDong && statusMoi != QStringLiteral("Hoạt động")) {
        QSqlQuery count(db);
        count.prepare("SELECT COUNT(*) FROM dbo.TaiKhoan WITH (UPDLOCK, HOLDLOCK) "
                      "WHERE VaiTro IN (N'Admin', N'Quản lý') "
                      "AND TrangThai = N'Hoạt động' AND MaTK <> ?");
        count.addBindValue(qstr(maTK));
        if (!count.exec() || !count.next()) {
            reportSqlError("Kiem tra Admin hoat dong con lai that bai", count);
            count.finish(); db.rollback();
            return false;
        }
        const int remainingAdmins = count.value(0).toInt();
        count.finish();
        if (remainingAdmins == 0) {
            db.rollback();
            cout << "Khong the khoa Admin hoat dong cuoi cung!\n";
            return false;
        }
    }

    QSqlQuery update(db);
    update.prepare("UPDATE dbo.TaiKhoan SET TrangThai = ? WHERE MaTK = ?");
    update.addBindValue(statusMoi);
    update.addBindValue(qstr(maTK));
    if (!update.exec() || update.numRowsAffected() != 1) {
        reportSqlError("Cap nhat trang thai that bai", update);
        db.rollback();
        return false;
    }
    if (!db.commit()) {
        cerr << "Commit cap nhat trang thai that bai: " << db.lastError().text().toStdString() << '\n';
        db.rollback();
        return false;
    }
    napDanhSachSQL();
    cout << "Cap nhat trang thai SQL thanh cong!\n";
    return true;
}

const TaiKhoan* TaiKhoanService::timTheoMa(const string& maTK) const
{
    if (!napDanhSachSQL()) return nullptr;
    for (const TaiKhoan& tk : danhSachTaiKhoan) if (tk.getMaTK() == maTK) return &tk;
    return nullptr;
}

const TaiKhoan* TaiKhoanService::timTheoTenDangNhap(const string& tenDangNhap) const
{
    if (!napDanhSachSQL()) return nullptr;
    for (const TaiKhoan& tk : danhSachTaiKhoan) if (tk.getTenDangNhap() == tenDangNhap) return &tk;
    return nullptr;
}

void TaiKhoanService::hienThiDanhSach() const
{
    if (!napDanhSachSQL()) return;
    if (danhSachTaiKhoan.empty()) { cout << "Danh sach tai khoan rong!\n"; return; }
    cout << "\n========== DANH SACH TAI KHOAN =========\n";
    for (const TaiKhoan& tk : danhSachTaiKhoan) {
        cout << "Ma TK: " << tk.getMaTK() << '\n'
             << "Ten dang nhap: " << tk.getTenDangNhap() << '\n'
             << "Vai tro: " << tk.getVaiTro() << '\n'
             << "Ma nhan vien: " << tk.getMaNV() << '\n'
             << "Trang thai: " << tk.getTrangThai() << '\n'
             << "-----------------------------------------\n";
    }
}

const vector<TaiKhoan>& TaiKhoanService::getDanhSachTaiKhoan() const
{
    napDanhSachSQL();
    return danhSachTaiKhoan;
}
