#include "../include/ChiTietHoaDon.h"

ChiTietHoaDon::ChiTietHoaDon()
{
    maHD = "";
    maMon = "";
    soLuong = 0;
    donGia = 0;
}

ChiTietHoaDon::ChiTietHoaDon(string maHD, string maMon, int soLuong, double donGia)
{
    this->maHD = maHD;
    this->maMon = maMon;
    this->soLuong = soLuong;
    this->donGia = donGia;
}

string ChiTietHoaDon::getMaHD() const
{
    return maHD;
}

string ChiTietHoaDon::getMaMon() const
{
    return maMon;
}

int ChiTietHoaDon::getSoLuong() const
{
    return soLuong;
}

double ChiTietHoaDon::getDonGia() const
{
    return donGia;
}

void ChiTietHoaDon::setMaHD(string maHD)
{
    this->maHD = maHD;
}

void ChiTietHoaDon::setMaMon(string maMon)
{
    this->maMon = maMon;
}

void ChiTietHoaDon::setSoLuong(int soLuong)
{
    this->soLuong = soLuong;
}

void ChiTietHoaDon::setDonGia(double donGia)
{
    this->donGia = donGia;
}

bool ChiTietHoaDon::hopLe() const
{
    return !maHD.empty() && !maMon.empty() && soLuong > 0 && donGia >= 0;
}
