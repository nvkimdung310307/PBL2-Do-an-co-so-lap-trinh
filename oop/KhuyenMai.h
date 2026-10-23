#ifndef KHUYENMAI_H
#define KHUYENMAI_H

#include <iostream>
#include <string>
using namespace std;

class KhuyenMai
{
private:
    string maKM;
    string tenKM;
    double phanTramGiam;
    string ngayBatDau;
    string ngayKetThuc;
    double dieuKien;
    string trangThai;

public:
    KhuyenMai();
    KhuyenMai(string maKM, string tenKM, double phanTramGiam, string ngayBatDau, string ngayKetThuc, double dieuKien, string trangThai);

    string getMaKM() const;
    string getTenKM() const;
    double getPhanTramGiam() const;
    string getNgayBatDau() const;
    string getNgayKetThuc() const;
    double getDieuKien() const;
    string getTrangThai() const;

    void setMaKM(string maKM);
    void setTenKM(string tenKM);
    void setPhanTramGiam(double phanTramGiam);
    void setNgayBatDau(string ngayBatDau);
    void setNgayKetThuc(string ngayKetThuc);
    void setDieuKien(double dieuKien);
    void setTrangThai(string trangThai);

    bool hopLe() const;
};

#endif