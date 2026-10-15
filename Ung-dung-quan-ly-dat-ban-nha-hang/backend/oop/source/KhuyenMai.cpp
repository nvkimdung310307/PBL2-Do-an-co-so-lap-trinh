#include "../include/KhuyenMai.h"

KhuyenMai::KhuyenMai()
{
    maKM = "";
    tenKM = "";
    phanTramGiam = 0;
    ngayBatDau = "";
    ngayKetThuc = "";
    dieuKien = 0;
    trangThai = "";
}

KhuyenMai::KhuyenMai(string maKM, string tenKM, double phanTramGiam, string ngayBatDau, string ngayKetThuc, double dieuKien, string trangThai)
{
    this->maKM = maKM;
    this->tenKM = tenKM;
    this->phanTramGiam = phanTramGiam;
    this->ngayBatDau = ngayBatDau;
    this->ngayKetThuc = ngayKetThuc;
    this->dieuKien = dieuKien;
    this->trangThai = trangThai;
}

string KhuyenMai::getMaKM() const
{
    return maKM;
}

string KhuyenMai::getTenKM() const
{
    return tenKM;
}

double KhuyenMai::getPhanTramGiam() const
{
    return phanTramGiam;
}

string KhuyenMai::getNgayBatDau() const
{
    return ngayBatDau;
}

string KhuyenMai::getNgayKetThuc() const
{
    return ngayKetThuc;
}

double KhuyenMai::getDieuKien() const
{
    return dieuKien;
}

string KhuyenMai::getTrangThai() const
{
    return trangThai;
}

void KhuyenMai::setMaKM(string maKM)
{
    this->maKM = maKM;
}

void KhuyenMai::setTenKM(string tenKM)
{
    this->tenKM = tenKM;
}

void KhuyenMai::setPhanTramGiam(double phanTramGiam)
{
    this->phanTramGiam = phanTramGiam;
}

void KhuyenMai::setNgayBatDau(string ngayBatDau)
{
    this->ngayBatDau = ngayBatDau;
}

void KhuyenMai::setNgayKetThuc(string ngayKetThuc)
{
    this->ngayKetThuc = ngayKetThuc;
}

void KhuyenMai::setDieuKien(double dieuKien)
{
    this->dieuKien = dieuKien;
}

void KhuyenMai::setTrangThai(string trangThai)
{
    this->trangThai = trangThai;
}

bool KhuyenMai::hopLe() const
{
    return !maKM.empty() && !tenKM.empty() && phanTramGiam >= 0 && phanTramGiam <= 100 && !ngayBatDau.empty() && !ngayKetThuc.empty() && dieuKien >= 0;
}
