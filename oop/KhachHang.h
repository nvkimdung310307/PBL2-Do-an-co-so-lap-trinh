#ifndef KHACHHANG_H
#define KHACHHANG_H

#include <iostream>
#include <string>
using namespace std;

class KhachHang
{
private:
    string maKH;
    string hoTen;
    string soDienThoai;
    string email;
    string diaChi;

public:
    KhachHang();
    KhachHang(string maKH, string hoTen, string soDienThoai, string email, string diaChi);

    string getMaKH() const;
    string getHoTen() const;
    string getSoDienThoai() const;
    string getEmail() const;
    string getDiaChi() const;

    void setMaKH(string maKH);
    void setHoTen(string hoTen);
    void setSoDienThoai(string soDienThoai);
    void setEmail(string email);
    void setDiaChi(string diaChi);

    bool hopLe() const;
};

#endif