#include "../include/TaiKhoan.h"

TaiKhoan::TaiKhoan()
{
    maTK = "";
    tenDangNhap = "";
    matKhau = "";
    vaiTro = "";
    maNV = "";
    trangThai = "";
}

TaiKhoan::TaiKhoan(string maTK, string tenDangNhap, string matKhau, string vaiTro, string maNV, string trangThai)
{
    this->maTK = maTK;
    this->tenDangNhap = tenDangNhap;
    this->matKhau = matKhau;
    this->vaiTro = vaiTro;
    this->maNV = maNV;
    this->trangThai = trangThai;
}

string TaiKhoan::getMaTK() const
{
    return maTK;
}

string TaiKhoan::getTenDangNhap() const
{
    return tenDangNhap;
}

string TaiKhoan::getMatKhau() const
{
    return matKhau;
}

string TaiKhoan::getVaiTro() const
{
    return vaiTro;
}

string TaiKhoan::getMaNV() const
{
    return maNV;
}

string TaiKhoan::getTrangThai() const
{
    return trangThai;
}

void TaiKhoan::setMaTK(string maTK)
{
    this->maTK = maTK;
}

void TaiKhoan::setTenDangNhap(string tenDangNhap)
{
    this->tenDangNhap = tenDangNhap;
}

void TaiKhoan::setMatKhau(string matKhau)
{
    this->matKhau = matKhau;
}

void TaiKhoan::setVaiTro(string vaiTro)
{
    this->vaiTro = vaiTro;
}

void TaiKhoan::setMaNV(string maNV)
{
    this->maNV = maNV;
}

void TaiKhoan::setTrangThai(string trangThai)
{
    this->trangThai = trangThai;
}

bool TaiKhoan::hopLe() const
{
    return !maTK.empty() && !tenDangNhap.empty() && !matKhau.empty() && !vaiTro.empty() && !maNV.empty();
}
