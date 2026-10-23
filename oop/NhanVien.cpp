#include "NhanVien.h"

NhanVien::NhanVien()
{
    maNV = "";
    hoTen = "";
    gioiTinh = "";
    ngaySinh = "";
    soDienThoai = "";
    email = "";
    chucVu = "";
    luong = 0;
    trangThai = "";
}

NhanVien::NhanVien(string maNV, string hoTen, string gioiTinh, string ngaySinh, string soDienThoai, string email, string chucVu, double luong, string trangThai)
{
    this->maNV = maNV;
    this->hoTen = hoTen;
    this->gioiTinh = gioiTinh;
    this->ngaySinh = ngaySinh;
    this->soDienThoai = soDienThoai;
    this->email = email;
    this->chucVu = chucVu;
    this->luong = luong;
    this->trangThai = trangThai;
}

string NhanVien::getMaNV() const
{
    return maNV;
}

string NhanVien::getHoTen() const
{
    return hoTen;
}

string NhanVien::getGioiTinh() const
{
    return gioiTinh;
}

string NhanVien::getNgaySinh() const
{
    return ngaySinh;
}

string NhanVien::getSoDienThoai() const
{
    return soDienThoai;
}

string NhanVien::getEmail() const
{
    return email;
}

string NhanVien::getChucVu() const
{
    return chucVu;
}

double NhanVien::getLuong() const
{
    return luong;
}

string NhanVien::getTrangThai() const
{
    return trangThai;
}

void NhanVien::setMaNV(string maNV)
{
    this->maNV = maNV;
}

void NhanVien::setHoTen(string hoTen)
{
    this->hoTen = hoTen;
}

void NhanVien::setGioiTinh(string gioiTinh)
{
    this->gioiTinh = gioiTinh;
}

void NhanVien::setNgaySinh(string ngaySinh)
{
    this->ngaySinh = ngaySinh;
}

void NhanVien::setSoDienThoai(string soDienThoai)
{
    this->soDienThoai = soDienThoai;
}

void NhanVien::setEmail(string email)
{
    this->email = email;
}

void NhanVien::setChucVu(string chucVu)
{
    this->chucVu = chucVu;
}

void NhanVien::setLuong(double luong)
{
    this->luong = luong;
}

void NhanVien::setTrangThai(string trangThai)
{
    this->trangThai = trangThai;
}

bool NhanVien::hopLe() const
{
    return !maNV.empty() && !hoTen.empty() && luong >= 0;
}