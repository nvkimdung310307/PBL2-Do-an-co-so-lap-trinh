#include "DatBan.h"

DatBan::DatBan()
{
    maDatBan = "";
    maKH = "";
    maBan = "";
    maNV = "";
    ngayDat = "";
    gioDat = "";
    soNguoi = 0;
    trangThai = "";
    ghiChu = "";
}

DatBan::DatBan(string maDatBan, string maKH, string maBan, string maNV, string ngayDat, string gioDat, int soNguoi, string trangThai, string ghiChu)
{
    this->maDatBan = maDatBan;
    this->maKH = maKH;
    this->maBan = maBan;
    this->maNV = maNV;
    this->ngayDat = ngayDat;
    this->gioDat = gioDat;
    this->soNguoi = soNguoi;
    this->trangThai = trangThai;
    this->ghiChu = ghiChu;
}

string DatBan::getMaDatBan() const
{
    return maDatBan;
}

string DatBan::getMaKH() const
{
    return maKH;
}

string DatBan::getMaBan() const
{
    return maBan;
}

string DatBan::getMaNV() const
{
    return maNV;
}

string DatBan::getNgayDat() const
{
    return ngayDat;
}

string DatBan::getGioDat() const
{
    return gioDat;
}

int DatBan::getSoNguoi() const
{
    return soNguoi;
}

string DatBan::getTrangThai() const
{
    return trangThai;
}

string DatBan::getGhiChu() const
{
    return ghiChu;
}

void DatBan::setMaDatBan(string maDatBan)
{
    this->maDatBan = maDatBan;
}

void DatBan::setMaKH(string maKH)
{
    this->maKH = maKH;
}

void DatBan::setMaBan(string maBan)
{
    this->maBan = maBan;
}

void DatBan::setMaNV(string maNV)
{
    this->maNV = maNV;
}

void DatBan::setNgayDat(string ngayDat)
{
    this->ngayDat = ngayDat;
}

void DatBan::setGioDat(string gioDat)
{
    this->gioDat = gioDat;
}

void DatBan::setSoNguoi(int soNguoi)
{
    this->soNguoi = soNguoi;
}

void DatBan::setTrangThai(string trangThai)
{
    this->trangThai = trangThai;
}

void DatBan::setGhiChu(string ghiChu)
{
    this->ghiChu = ghiChu;
}

bool DatBan::hopLe() const
{
    return !maDatBan.empty() && !maKH.empty() && !maBan.empty() && !ngayDat.empty() && !gioDat.empty() && soNguoi > 0;
}