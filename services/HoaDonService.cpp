#include "HoaDonService.h"
#include "../DatabaseManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

namespace {
QString qstr(const string& s) { return QString::fromUtf8(s.c_str()); }
string str(const QVariant& v) { return v.toString().toUtf8().toStdString(); }
void sqlError(const QString& where, const QSqlError& e) {
    cerr << where.toUtf8().constData() << ": " << e.text().toUtf8().constData() << '\n';
}
bool hoaDonDaDong(const string& s) {
    return s == "Đã thanh toán" || s == "Da thanh toan" || s == "Đã hủy" || s == "Da huy";
}
}

bool HoaDonService::napDanhSachSQL() const {
    if (!DatabaseManager::connect()) return false;
    vector<HoaDon> hds;
    vector<ChiTietHoaDon> cts;
    QSqlQuery q(DatabaseManager::database());
    if (!q.exec("SELECT MaHD,MaDatBan,ISNULL(MaNV,''),ISNULL(MaKM,''),CONVERT(varchar(19),NgayLap,120),TongTien,TienGiam,ThanhTien,ISNULL(PhuongThucThanhToan,N''),TrangThai FROM dbo.HoaDon ORDER BY NgayLap DESC,MaHD")) {
        sqlError("Tai danh sach hoa don", q.lastError()); return false;
    }
    while (q.next()) hds.emplace_back(str(q.value(0)),str(q.value(1)),str(q.value(2)),str(q.value(3)),str(q.value(4)),q.value(5).toDouble(),q.value(6).toDouble(),q.value(7).toDouble(),str(q.value(8)),str(q.value(9)));
    q.finish();
    if (!q.exec("SELECT MaHD,MaMon,SoLuong,DonGia FROM dbo.ChiTietHoaDon ORDER BY MaHD,MaMon")) {
        sqlError("Tai chi tiet hoa don", q.lastError()); return false;
    }
    while (q.next()) cts.emplace_back(str(q.value(0)),str(q.value(1)),q.value(2).toInt(),q.value(3).toDouble());
    danhSachHoaDon = std::move(hds); danhSachChiTiet = std::move(cts);
    return true;
}

const HoaDon* HoaDonService::timHoaDon(const string& maHD) const {
    if (!napDanhSachSQL()) return nullptr;
    for (const auto& hd : danhSachHoaDon) if (hd.getMaHD()==maHD) return &hd;
    return nullptr;
}

bool HoaDonService::taoHoaDon(const HoaDon& hd) {
    if (!hd.hopLe() || hd.getMaHD().empty() || hd.getMaDatBan().empty()) { cerr << "Thong tin hoa don khong hop le!\n"; return false; }
    if (!DatabaseManager::connect()) return false;
    QSqlQuery q(DatabaseManager::database());
    q.prepare("INSERT INTO dbo.HoaDon(MaHD,MaDatBan,MaNV,MaKM,NgayLap,TongTien,TienGiam,ThanhTien,PhuongThucThanhToan,TrangThai) VALUES(:id,:booking,NULLIF(:nv,''),NULLIF(:km,''),COALESCE(TRY_CONVERT(datetime,:date,120),GETDATE()),0,0,0,NULL,N'Chưa thanh toán')");
    q.bindValue(":id",qstr(hd.getMaHD())); q.bindValue(":booking",qstr(hd.getMaDatBan())); q.bindValue(":nv",qstr(hd.getMaNV())); q.bindValue(":km",qstr(hd.getMaKM())); q.bindValue(":date",qstr(hd.getNgayLap()));
    if (!q.exec()) { sqlError("Tao hoa don",q.lastError()); return false; }
    napDanhSachSQL(); return true;
}

bool HoaDonService::themChiTiet(const ChiTietHoaDon& ct) {
    if (!ct.hopLe() || ct.getMaHD().empty() || ct.getMaMon().empty() || ct.getSoLuong()<=0 || !std::isfinite(ct.getDonGia()) || ct.getDonGia()<0) return false;
    if (!DatabaseManager::connect()) return false;
    QSqlDatabase db=DatabaseManager::database();
    if (!db.transaction()) return false;
    QSqlQuery q(db);
    q.prepare("IF NOT EXISTS(SELECT 1 FROM dbo.HoaDon WITH (UPDLOCK,HOLDLOCK) WHERE MaHD=:id AND TrangThai=N'Chưa thanh toán') THROW 50101,N'Hoa don khong ton tai hoac da dong.',1; INSERT dbo.ChiTietHoaDon(MaHD,MaMon,SoLuong,DonGia) VALUES(:id,:mon,:qty,:price)");
    q.bindValue(":id",qstr(ct.getMaHD())); q.bindValue(":mon",qstr(ct.getMaMon())); q.bindValue(":qty",ct.getSoLuong()); q.bindValue(":price",ct.getDonGia());
    if (!q.exec()) { sqlError("Them chi tiet hoa don",q.lastError()); db.rollback(); return false; }
    if (!db.commit()) { sqlError("Luu chi tiet hoa don",db.lastError()); db.rollback(); return false; }
    napDanhSachSQL(); return true;
}

bool HoaDonService::suaChiTiet(const string& maHD,const string& maMon,int soLuongMoi) {
    if (soLuongMoi<=0 || !DatabaseManager::connect()) return false;
    QSqlQuery q(DatabaseManager::database());
    q.prepare("UPDATE ct SET SoLuong=:qty FROM dbo.ChiTietHoaDon ct JOIN dbo.HoaDon h ON h.MaHD=ct.MaHD WHERE ct.MaHD=:id AND ct.MaMon=:mon AND h.TrangThai=N'Chưa thanh toán'");
    q.bindValue(":qty",soLuongMoi); q.bindValue(":id",qstr(maHD)); q.bindValue(":mon",qstr(maMon));
    if (!q.exec()) { sqlError("Sua chi tiet hoa don",q.lastError()); return false; }
    if (q.numRowsAffected()!=1) return false;
    napDanhSachSQL(); return true;
}

bool HoaDonService::xoaChiTiet(const string& maHD,const string& maMon) {
    if (!DatabaseManager::connect()) return false;
    QSqlQuery q(DatabaseManager::database());
    q.prepare("DELETE ct FROM dbo.ChiTietHoaDon ct JOIN dbo.HoaDon h ON h.MaHD=ct.MaHD WHERE ct.MaHD=:id AND ct.MaMon=:mon AND h.TrangThai=N'Chưa thanh toán'");
    q.bindValue(":id",qstr(maHD)); q.bindValue(":mon",qstr(maMon));
    if (!q.exec()) { sqlError("Xoa chi tiet hoa don",q.lastError()); return false; }
    if (q.numRowsAffected()!=1) return false;
    napDanhSachSQL(); return true;
}

bool HoaDonService::tinhTien(const string& maHD,double tienGiam) {
    if (!std::isfinite(tienGiam)||tienGiam<0||!DatabaseManager::connect()) return false;
    QSqlQuery q(DatabaseManager::database());
    q.prepare("EXEC dbo.sp_TinhTongHoaDon @MaHD=:id, @TienGiamMoi=:discount");
    q.bindValue(":id",qstr(maHD)); q.bindValue(":discount",tienGiam);
    if (!q.exec()) { sqlError("Tinh tien hoa don",q.lastError()); return false; }
    q.finish();
    napDanhSachSQL(); return true;
}


bool HoaDonService::gopHoaDon(const string& maHDNguon, const string& maHDDich) {
    if (maHDNguon.empty() || maHDDich.empty() || maHDNguon == maHDDich) {
        cerr << "Ma hoa don nguon/dich khong hop le.\n"; return false;
    }
    if (!DatabaseManager::connect()) return false;
    QSqlDatabase db = DatabaseManager::database();
    if (!db.transaction()) { cerr << "Khong tao duoc transaction SQL.\n"; return false; }
    QSqlQuery q(db);
    q.prepare("SELECT MaHD,TrangThai FROM dbo.HoaDon WITH (UPDLOCK,HOLDLOCK) WHERE MaHD IN (?,?)");
    q.addBindValue(qstr(maHDNguon)); q.addBindValue(qstr(maHDDich));
    if (!q.exec()) { sqlError("Kiem tra hoa don gop",q.lastError()); db.rollback(); return false; }
    int found=0; bool sourceOpen=false, targetOpen=false;
    while(q.next()) { ++found; const string id=str(q.value(0)), status=str(q.value(1));
        if (status!="Chưa thanh toán" && status!="Chua thanh toan") { sqlError("Hoa don da dong", QSqlError()); db.rollback(); return false; }
        if(id==maHDNguon) sourceOpen=true; if(id==maHDDich) targetOpen=true;
    }
    q.finish();
    if(found!=2 || !sourceOpen || !targetOpen) { cerr << "Khong tim thay du hai hoa don chua thanh toan.\n"; db.rollback(); return false; }
    // Trung ma mon: cong don so luong; khong ghi de don gia cua hoa don dich.
    q.prepare("IF EXISTS(SELECT 1 FROM dbo.ChiTietHoaDon s JOIN dbo.ChiTietHoaDon d ON d.MaHD=? AND d.MaMon=s.MaMon WHERE s.MaHD=?) BEGIN UPDATE d SET SoLuong=d.SoLuong+s.SoLuong FROM dbo.ChiTietHoaDon d JOIN dbo.ChiTietHoaDon s ON s.MaMon=d.MaMon WHERE d.MaHD=? AND s.MaHD=?; DELETE s FROM dbo.ChiTietHoaDon s WHERE s.MaHD=? AND EXISTS(SELECT 1 FROM dbo.ChiTietHoaDon d WHERE d.MaHD=? AND d.MaMon=s.MaMon); END; UPDATE dbo.ChiTietHoaDon SET MaHD=? WHERE MaHD=? AND NOT EXISTS(SELECT 1 FROM dbo.ChiTietHoaDon d WHERE d.MaHD=? AND d.MaMon=ChiTietHoaDon.MaMon);");
    q.addBindValue(qstr(maHDDich)); q.addBindValue(qstr(maHDNguon)); q.addBindValue(qstr(maHDDich)); q.addBindValue(qstr(maHDNguon)); q.addBindValue(qstr(maHDNguon)); q.addBindValue(qstr(maHDDich)); q.addBindValue(qstr(maHDDich)); q.addBindValue(qstr(maHDNguon)); q.addBindValue(qstr(maHDDich));
    if(!q.exec()) { sqlError("Chuyen chi tiet khi gop hoa don",q.lastError()); db.rollback(); return false; }
    q.finish();
    q.prepare("UPDATE dbo.HoaDon SET TrangThai=N'Đã hủy',TongTien=0,TienGiam=0,ThanhTien=0 WHERE MaHD=? AND TrangThai=N'Chưa thanh toán'"); q.addBindValue(qstr(maHDNguon));
    if(!q.exec() || q.numRowsAffected()!=1) { sqlError("Dong hoa don nguon sau khi gop",q.lastError()); db.rollback(); return false; }
    q.finish();
    q.prepare("EXEC dbo.sp_TinhTongHoaDon @MaHD=?, @TienGiamMoi=0"); q.addBindValue(qstr(maHDDich));
    if(!q.exec()) { sqlError("Tinh lai hoa don dich",q.lastError()); db.rollback(); return false; }
    q.finish();
    if(!db.commit()) { sqlError("Luu gop hoa don",db.lastError()); db.rollback(); return false; }
    napDanhSachSQL(); return true;
}

bool HoaDonService::tachHoaDon(const string& maHDNguon, const string& maHDDich, const string& maMon, int soLuong) {
    if(maHDNguon.empty() || maHDDich.empty() || maHDNguon==maHDDich || maMon.empty() || soLuong<=0) return false;
    if(!DatabaseManager::connect()) return false;
    QSqlDatabase db=DatabaseManager::database();
    if(!db.transaction()) return false;
    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) FROM dbo.HoaDon WITH (UPDLOCK,HOLDLOCK) WHERE MaHD IN (?,?) AND TrangThai=N'Chưa thanh toán'");
    q.addBindValue(qstr(maHDNguon)); q.addBindValue(qstr(maHDDich));
    if(!q.exec() || !q.next() || q.value(0).toInt()!=2) { sqlError("Kiem tra hoa don tach",q.lastError()); db.rollback(); return false; }
    q.finish();
    q.prepare("SELECT SoLuong,DonGia FROM dbo.ChiTietHoaDon WITH (UPDLOCK,HOLDLOCK) WHERE MaHD=? AND MaMon=?");
    q.addBindValue(qstr(maHDNguon)); q.addBindValue(qstr(maMon));
    if(!q.exec() || !q.next()) { sqlError("Tim mon can tach",q.lastError()); db.rollback(); return false; }
    int current=q.value(0).toInt(); double price=q.value(1).toDouble(); q.finish();
    if(soLuong>current) { cerr << "So luong tach vuot qua so luong hien co.\n"; db.rollback(); return false; }
    // Kiem tra ton tai rieng de tranh truy van IF/ELSE tham so khong duoc su dung.
    QSqlQuery exists(db); exists.prepare("SELECT COUNT(*) FROM dbo.ChiTietHoaDon WHERE MaHD=? AND MaMon=?"); exists.addBindValue(qstr(maHDDich)); exists.addBindValue(qstr(maMon));
    if(!exists.exec() || !exists.next()) { sqlError("Kiem tra mon hoa don dich",exists.lastError()); db.rollback(); return false; }
    bool has=exists.value(0).toInt()>0; exists.finish();
    if(has) { q.prepare("UPDATE dbo.ChiTietHoaDon SET SoLuong=SoLuong+? WHERE MaHD=? AND MaMon=?"); q.addBindValue(soLuong); q.addBindValue(qstr(maHDDich)); q.addBindValue(qstr(maMon)); }
    else { q.prepare("INSERT dbo.ChiTietHoaDon(MaHD,MaMon,SoLuong,DonGia) VALUES(?,?,?,?)"); q.addBindValue(qstr(maHDDich)); q.addBindValue(qstr(maMon)); q.addBindValue(soLuong); q.addBindValue(price); }
    if(!q.exec()) { sqlError("Them mon sang hoa don tach",q.lastError()); db.rollback(); return false; }
    q.finish();
    if(soLuong==current) { q.prepare("DELETE FROM dbo.ChiTietHoaDon WHERE MaHD=? AND MaMon=?"); q.addBindValue(qstr(maHDNguon)); q.addBindValue(qstr(maMon)); }
    else { q.prepare("UPDATE dbo.ChiTietHoaDon SET SoLuong=SoLuong-? WHERE MaHD=? AND MaMon=?"); q.addBindValue(soLuong); q.addBindValue(qstr(maHDNguon)); q.addBindValue(qstr(maMon)); }
    if(!q.exec()) { sqlError("Cap nhat hoa don nguon",q.lastError()); db.rollback(); return false; }
    q.finish();
    for(const string& id : {maHDNguon,maHDDich}) { q.prepare("EXEC dbo.sp_TinhTongHoaDon @MaHD=?, @TienGiamMoi=0"); q.addBindValue(qstr(id)); if(!q.exec()) { sqlError("Tinh lai tien sau tach hoa don",q.lastError()); db.rollback(); return false; } q.finish(); }
    if(!db.commit()) { sqlError("Luu tach hoa don",db.lastError()); db.rollback(); return false; }
    napDanhSachSQL(); return true;
}

bool HoaDonService::thanhToan(const string& maHD,const string& phuongThuc) {
    QString method=qstr(phuongThuc);
    if (phuongThuc=="Tien mat") method=QString::fromUtf8("Tiền mặt");
    else if (phuongThuc=="Chuyen khoan") method=QString::fromUtf8("Chuyển khoản");
    else if (phuongThuc=="Vi dien tu") method=QString::fromUtf8("Ví điện tử");
    else if (phuongThuc=="QR") method=QStringLiteral("QR");
    if (method!=QString::fromUtf8("Tiền mặt") && method!=QString::fromUtf8("Chuyển khoản") && method!=QString::fromUtf8("Ví điện tử") && method!=QStringLiteral("QR")) { cerr << "Phuong thuc thanh toan khong hop le!\n"; return false; }
    if (!DatabaseManager::connect()) return false;
    QSqlQuery q(DatabaseManager::database());
    q.prepare("EXEC dbo.sp_ThanhToanHoaDon @MaHD=:id,@PhuongThucThanhToan=:method");
    q.bindValue(":id",qstr(maHD)); q.bindValue(":method",method);
    if (!q.exec()) { sqlError("Thanh toan hoa don",q.lastError()); return false; }
    q.finish();
    napDanhSachSQL(); return true;
}

void HoaDonService::hienThiDanhSach() const {
    if (!napDanhSachSQL()) { cerr << "Khong doc duoc danh sach hoa don tu SQL.\n"; return; }
    if (danhSachHoaDon.empty()) { cout << "Danh sach hoa don dang rong!\n"; return; }
    for (const auto& hd:danhSachHoaDon) cout << "\nMa hoa don: "<<hd.getMaHD()<<"\nMa dat ban: "<<hd.getMaDatBan()<<"\nNgay lap: "<<hd.getNgayLap()<<"\nTong tien: "<<hd.getTongTien()<<"\nTien giam: "<<hd.getTienGiam()<<"\nThanh tien: "<<hd.getThanhTien()<<"\nPhuong thuc: "<<hd.getPhuongThucThanhToan()<<"\nTrang thai: "<<hd.getTrangThai()<<"\n-------------------------------\n";
}

void HoaDonService::hienThiChiTiet(const string& maHD) const {
    if (!napDanhSachSQL()) { cerr << "Khong doc duoc chi tiet hoa don tu SQL.\n"; return; }
    bool found=false; cout << "\n===== CHI TIET HOA DON "<<maHD<<" =====\n";
    for (const auto& ct:danhSachChiTiet) if (ct.getMaHD()==maHD) { found=true; cout<<"Ma mon: "<<ct.getMaMon()<<"\nSo luong: "<<ct.getSoLuong()<<"\nDon gia: "<<ct.getDonGia()<<"\nThanh tien: "<<ct.getSoLuong()*ct.getDonGia()<<"\n---------------------\n"; }
    if (!found) cout << "Hoa don khong ton tai hoac chua co mon an!\n";
}
const vector<HoaDon>& HoaDonService::getDanhSachHoaDon() const { napDanhSachSQL(); return danhSachHoaDon; }
const vector<ChiTietHoaDon>& HoaDonService::getDanhSachChiTiet() const { napDanhSachSQL(); return danhSachChiTiet; }
