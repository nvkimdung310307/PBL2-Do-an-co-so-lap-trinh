#include "KhachHang.h"

KhachHang::KhachHang()
{
    maKH = "";
    hoTen = "";
    soDienThoai = "";
    email = "";
    diaChi = "";
}

KhachHang::KhachHang(string maKH, string hoTen, string soDienThoai, string email, string diaChi)
{
    this->maKH = maKH;
    this->hoTen = hoTen;
    this->soDienThoai = soDienThoai;
    this->email = email;
    this->diaChi = diaChi;
}

string KhachHang::getMaKH() const
{
    return maKH;
}

string KhachHang::getHoTen() const
{
    return hoTen;
}

string KhachHang::getSoDienThoai() const
{
    return soDienThoai;
}

string KhachHang::getEmail() const
{
    return email;
}

string KhachHang::getDiaChi() const
{
    return diaChi;
}

void KhachHang::setMaKH(string maKH)
{
    this->maKH = maKH;
}

void KhachHang::setHoTen(string hoTen)
{
    this->hoTen = hoTen;
}

void KhachHang::setSoDienThoai(string soDienThoai)
{
    this->soDienThoai = soDienThoai;
}

void KhachHang::setEmail(string email)
{
    this->email = email;
}

void KhachHang::setDiaChi(string diaChi)
{
    this->diaChi = diaChi;
}

bool KhachHang::hopLe() const
{
    return !maKH.empty() && !hoTen.empty() && !soDienThoai.empty();
}