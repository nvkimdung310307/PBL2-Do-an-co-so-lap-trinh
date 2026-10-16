
#include "KhachCho.h"

KhachCho::KhachCho()
{
    maKhachCho = "";
    tenKhach = "";
    soDienThoai = "";
    soLuongKhach = 0;
    khuVucMongMuon = "";
    trangThai = "DangCho";
}

KhachCho::KhachCho(string ma, string ten, string sdt,
                   int soLuong, string khuVuc)
{
    maKhachCho = ma;
    tenKhach = ten;
    soDienThoai = sdt;
    soLuongKhach = soLuong;
    khuVucMongMuon = khuVuc;
    trangThai = "DangCho";
}

string KhachCho::getMaKhachCho() const
{
    return maKhachCho;
}

string KhachCho::getTenKhach() const
{
    return tenKhach;
}

string KhachCho::getSoDienThoai() const
{
    return soDienThoai;
}

int KhachCho::getSoLuongKhach() const
{
    return soLuongKhach;
}

string KhachCho::getKhuVucMongMuon() const
{
    return khuVucMongMuon;
}

string KhachCho::getTrangThai() const
{
    return trangThai;
}

void KhachCho::setTrangThai(string trangThai)
{
    this->trangThai = trangThai;
}

void KhachCho::hienThi() const
{
    cout << "Ma khach cho: " << maKhachCho << '\n';
    cout << "Ten khach: " << tenKhach << '\n';
    cout << "So dien thoai: " << soDienThoai << '\n';
    cout << "So luong khach: " << soLuongKhach << '\n';
    cout << "Khu vuc mong muon: " << khuVucMongMuon << '\n';
    cout << "Trang thai: " << trangThai << '\n';
}
