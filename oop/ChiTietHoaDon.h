#ifndef CHITIETHOADON_H
#define CHITIETHOADON_H

#include <iostream>
#include <string>
using namespace std;

class ChiTietHoaDon
{
private:
    string maHD;
    string maMon;
    int soLuong;
    double donGia;

public:
    ChiTietHoaDon();
    ChiTietHoaDon(string maHD, string maMon, int soLuong, double donGia);

    string getMaHD() const;
    string getMaMon() const;
    int getSoLuong() const;
    double getDonGia() const;

    void setMaHD(string maHD);
    void setMaMon(string maMon);
    void setSoLuong(int soLuong);
    void setDonGia(double donGia);

    bool hopLe() const;
};

#endif