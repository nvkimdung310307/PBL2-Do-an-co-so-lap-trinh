#include "DatCocService.h"
#include "../DatabaseManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <cmath>
#include <iostream>

namespace {
QString qstr(const string& s) { return QString::fromUtf8(s.c_str()); }
string str(const QVariant& v) { return v.toString().toUtf8().toStdString(); }
void sqlError(const char* where, const QSqlError& e) { cerr << where << ": " << e.text().toUtf8().constData() << '\n'; }
}

bool DatCocService::themDatCoc(const DatCoc& dc) {
    if (dc.getMaDatCoc().empty() || dc.getMaDatBan().empty() || !std::isfinite(dc.getTienCoc()) || dc.getTienCoc()<=0) {
        cerr << "Thong tin dat coc khong hop le!\n"; return false;
    }
    if (!DatabaseManager::connect()) return false;
    QSqlQuery q(DatabaseManager::database());
    q.prepare("EXEC dbo.sp_ThemDatCoc @MaDatCoc=:id,@MaDatBan=:booking,@TienCoc=:amount,@NgayDatCoc=:date");
    q.bindValue(":id",qstr(dc.getMaDatCoc())); q.bindValue(":booking",qstr(dc.getMaDatBan())); q.bindValue(":amount",dc.getTienCoc()); q.bindValue(":date",qstr(dc.getNgayDatCoc()));
    if (!q.exec()) { sqlError("Them dat coc",q.lastError()); return false; }
    cout << "Them tien coc thanh cong!\n"; return true;
}

bool DatCocService::hoanCoc(const string& id) {
    if (!DatabaseManager::connect()) return false;
    QSqlQuery q(DatabaseManager::database()); q.prepare("EXEC dbo.sp_HoanDatCoc @MaDatCoc=:id"); q.bindValue(":id",qstr(id));
    if (!q.exec()) { sqlError("Hoan tien coc",q.lastError()); return false; }
    cout << "Da ghi nhan hoan tien coc!\n"; return true;
}

bool DatCocService::capNhatTienCoc(const string& id,double amount) {
    if (!std::isfinite(amount)||amount<=0||!DatabaseManager::connect()) return false;
    QSqlQuery q(DatabaseManager::database()); q.prepare("EXEC dbo.sp_CapNhatTienCoc @MaDatCoc=:id,@TienMoi=:amount"); q.bindValue(":id",qstr(id)); q.bindValue(":amount",amount);
    if (!q.exec()) { sqlError("Cap nhat tien coc",q.lastError()); return false; }
    return true;
}

void DatCocService::hienThiDanhSach() const {
    if (!DatabaseManager::connect()) { cerr << "Khong ket noi duoc SQL.\n"; return; }
    QSqlQuery q(DatabaseManager::database());
    if (!q.exec("EXEC dbo.sp_DanhSachDatCoc")) { sqlError("Doc danh sach dat coc",q.lastError()); return; }
    bool any=false;
    while (q.next()) {
        any=true;
        cout << "\nMa dat coc: " << str(q.value(0)) << "\nMa dat ban: " << str(q.value(1))
             << "\nTien coc: " << q.value(2).toDouble() << "\nNgay dat coc: " << str(q.value(3))
             << "\nTrang thai: " << str(q.value(4)) << "\n--------------------\n";
    }
    if (!any) cout << "Chua co du lieu dat coc!\n";
}

double DatCocService::tinhTongTienCoc() const {
    if (!DatabaseManager::connect()) return 0;
    QSqlQuery q(DatabaseManager::database());
    if (!q.exec("SELECT COALESCE(SUM(TienCoc),0) FROM dbo.DatCoc WHERE TrangThai='DaCoc'")) { sqlError("Tinh tong tien coc",q.lastError()); return 0; }
    return q.next()?q.value(0).toDouble():0;
}

double DatCocService::tinhTienCoc(const string& booking) const {
    if (!DatabaseManager::connect()) return 0;
    QSqlQuery q(DatabaseManager::database()); q.prepare("SELECT COALESCE(SUM(TienCoc),0) FROM dbo.DatCoc WHERE MaDatBan=:booking AND TrangThai='DaCoc'"); q.bindValue(":booking",qstr(booking));
    if (!q.exec()) { sqlError("Tinh tien coc theo dat ban",q.lastError()); return 0; }
    return q.next()?q.value(0).toDouble():0;
}
