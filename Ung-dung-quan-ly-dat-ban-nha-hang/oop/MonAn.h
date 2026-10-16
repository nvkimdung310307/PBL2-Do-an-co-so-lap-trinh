#ifndef MONAN_H
#define MONAN_H

#include <iostream>
#include <string>
using namespace std;

class MonAn
{
private:
    string maMon;
    string tenMon;
    string loaiMon;
    double donGia;
    string donViTinh;
    string moTa;
    string hinhAnh;
    string trangThai;

public:
    MonAn();
    MonAn(string maMon, string tenMon, string loaiMon, double donGia, string donViTinh, string moTa, string hinhAnh, string trangThai);

    string getMaMon() const;
    string getTenMon() const;
    string getLoaiMon() const;
    double getDonGia() const;
    string getDonViTinh() const;
    string getMoTa() const;
    string getHinhAnh() const;
    string getTrangThai() const;

    void setMaMon(string maMon);
    void setTenMon(string tenMon);
    void setLoaiMon(string loaiMon);
    void setDonGia(double donGia);
    void setDonViTinh(string donViTinh);
    void setMoTa(string moTa);
    void setHinhAnh(string hinhAnh);
    void setTrangThai(string trangThai);

    bool hopLe() const;
};

#endif