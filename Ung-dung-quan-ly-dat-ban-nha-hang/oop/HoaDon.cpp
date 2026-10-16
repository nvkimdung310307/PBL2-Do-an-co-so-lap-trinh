#include "HoaDon.h"

HoaDon::HoaDon()
{
    maHD = "";
    maDatBan = "";
    maNV = "";
    maKM = "";
    ngayLap = "";
    tongTien = 0;
    tienGiam = 0;
    thanhTien = 0;
    phuongThucThanhToan = "";
    trangThai = "";
}

HoaDon::HoaDon(string maHD, string maDatBan, string maNV, string maKM, string ngayLap, double tongTien, double tienGiam, double thanhTien, string phuongThucThanhToan, string trangThai)
{
    this->maHD = maHD;
    this->maDatBan = maDatBan;
    this->maNV = maNV;
    this->maKM = maKM;
    this->ngayLap = ngayLap;
    this->tongTien = tongTien;
    this->tienGiam = tienGiam;
    this->thanhTien = thanhTien;
    this->phuongThucThanhToan = phuongThucThanhToan;
    this->trangThai = trangThai;
}

string HoaDon::getMaHD() const
{
    return maHD;
}

string HoaDon::getMaDatBan() const
{
    return maDatBan;
}

string HoaDon::getMaNV() const
{
    return maNV;
}

string HoaDon::getMaKM() const
{
    return maKM;
}

string HoaDon::getNgayLap() const
{
    return ngayLap;
}

double HoaDon::getTongTien() const
{
    return tongTien;
}

double HoaDon::getTienGiam() const
{
    return tienGiam;
}

double HoaDon::getThanhTien() const
{
    return thanhTien;
}

string HoaDon::getPhuongThucThanhToan() const
{
    return phuongThucThanhToan;
}

string HoaDon::getTrangThai() const
{
    return trangThai;
}

void HoaDon::setMaHD(string maHD)
{
    this->maHD = maHD;
}

void HoaDon::setMaDatBan(string maDatBan)
{
    this->maDatBan = maDatBan;
}

void HoaDon::setMaNV(string maNV)
{
    this->maNV = maNV;
}

void HoaDon::setMaKM(string maKM)
{
    this->maKM = maKM;
}

void HoaDon::setNgayLap(string ngayLap)
{
    this->ngayLap = ngayLap;
}

void HoaDon::setTongTien(double tongTien)
{
    this->tongTien = tongTien;
}

void HoaDon::setTienGiam(double tienGiam)
{
    this->tienGiam = tienGiam;
}

void HoaDon::setThanhTien(double thanhTien)
{
    this->thanhTien = thanhTien;
}

void HoaDon::setPhuongThucThanhToan(string phuongThucThanhToan)
{
    this->phuongThucThanhToan = phuongThucThanhToan;
}

void HoaDon::setTrangThai(string trangThai)
{
    this->trangThai = trangThai;
}

bool HoaDon::hopLe() const
{
    return !maHD.empty() && !maDatBan.empty() && tongTien >= 0 && tienGiam >= 0 && thanhTien >= 0;
}