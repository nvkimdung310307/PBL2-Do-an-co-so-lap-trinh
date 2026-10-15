#ifndef HOADON_H
#define HOADON_H

#include <iostream>
#include <string>
using namespace std;

class HoaDon
{
private:
    string maHD;
    string maDatBan;
    string maNV;
    string maKM;
    string ngayLap;
    double tongTien;
    double tienGiam;
    double thanhTien;
    string phuongThucThanhToan;
    string trangThai;

public:
    HoaDon();
    HoaDon(string maHD, string maDatBan, string maNV, string maKM, string ngayLap, double tongTien, double tienGiam, double thanhTien, string phuongThucThanhToan, string trangThai);

    string getMaHD() const;
    string getMaDatBan() const;
    string getMaNV() const;
    string getMaKM() const;
    string getNgayLap() const;
    double getTongTien() const;
    double getTienGiam() const;
    double getThanhTien() const;
    string getPhuongThucThanhToan() const;
    string getTrangThai() const;

    void setMaHD(string maHD);
    void setMaDatBan(string maDatBan);
    void setMaNV(string maNV);
    void setMaKM(string maKM);
    void setNgayLap(string ngayLap);
    void setTongTien(double tongTien);
    void setTienGiam(double tienGiam);
    void setThanhTien(double thanhTien);
    void setPhuongThucThanhToan(string phuongThucThanhToan);
    void setTrangThai(string trangThai);

    bool hopLe() const;
};

#endif
