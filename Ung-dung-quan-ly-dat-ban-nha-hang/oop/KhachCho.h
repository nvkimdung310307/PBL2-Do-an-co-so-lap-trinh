#pragma once
#include <iostream>
#include <string>
using namespace std;

class KhachCho
{
private:
    string maKhachCho;
    string tenKhach;
    string soDienThoai;
    int soLuongKhach;
    string khuVucMongMuon;
    string trangThai;

public:
    KhachCho();
    KhachCho(string ma, string ten, string sdt,
             int soLuong, string khuVuc);

    string getMaKhachCho() const;
    string getTenKhach() const;
    string getSoDienThoai() const;
    int getSoLuongKhach() const;
    string getKhuVucMongMuon() const;
    string getTrangThai() const;

    void setTrangThai(string trangThai);
    void hienThi() const;
};
