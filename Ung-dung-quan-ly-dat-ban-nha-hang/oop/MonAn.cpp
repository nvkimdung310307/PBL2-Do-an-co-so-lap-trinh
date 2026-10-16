#include "MonAn.h"

MonAn::MonAn()
{
    maMon = "";
    tenMon = "";
    loaiMon = "";
    donGia = 0;
    donViTinh = "";
    moTa = "";
    hinhAnh = "";
    trangThai = "";
}

MonAn::MonAn(string maMon, string tenMon, string loaiMon, double donGia, string donViTinh, string moTa, string hinhAnh, string trangThai)
{
    this->maMon = maMon;
    this->tenMon = tenMon;
    this->loaiMon = loaiMon;
    this->donGia = donGia;
    this->donViTinh = donViTinh;
    this->moTa = moTa;
    this->hinhAnh = hinhAnh;
    this->trangThai = trangThai;
}

string MonAn::getMaMon() const
{
    return maMon;
}

string MonAn::getTenMon() const
{
    return tenMon;
}

string MonAn::getLoaiMon() const
{
    return loaiMon;
}

double MonAn::getDonGia() const
{
    return donGia;
}

string MonAn::getDonViTinh() const
{
    return donViTinh;
}

string MonAn::getMoTa() const
{
    return moTa;
}

string MonAn::getHinhAnh() const
{
    return hinhAnh;
}

string MonAn::getTrangThai() const
{
    return trangThai;
}

void MonAn::setMaMon(string maMon)
{
    this->maMon = maMon;
}

void MonAn::setTenMon(string tenMon)
{
    this->tenMon = tenMon;
}

void MonAn::setLoaiMon(string loaiMon)
{
    this->loaiMon = loaiMon;
}

void MonAn::setDonGia(double donGia)
{
    this->donGia = donGia;
}

void MonAn::setDonViTinh(string donViTinh)
{
    this->donViTinh = donViTinh;
}

void MonAn::setMoTa(string moTa)
{
    this->moTa = moTa;
}

void MonAn::setHinhAnh(string hinhAnh)
{
    this->hinhAnh = hinhAnh;
}

void MonAn::setTrangThai(string trangThai)
{
    this->trangThai = trangThai;
}

bool MonAn::hopLe() const
{
    return !maMon.empty() && !tenMon.empty() && donGia >= 0;
}