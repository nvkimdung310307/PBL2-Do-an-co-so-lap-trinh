#include "DatCoc.h"

DatCoc::DatCoc()
{
    maDatCoc = "";
    maDatBan = "";
    tienCoc = 0;
    ngayDatCoc = "";
    trangThai = "DaCoc";
}

DatCoc::DatCoc(string maCoc, string maDatBan,
               double tienCoc, string ngayDatCoc)
{
    this->maDatCoc = maCoc;
    this->maDatBan = maDatBan;
    this->tienCoc = tienCoc;
    this->ngayDatCoc = ngayDatCoc;
    this->trangThai = "DaCoc";
}

string DatCoc::getMaDatCoc() const
{
    return maDatCoc;
}

string DatCoc::getMaDatBan() const
{
    return maDatBan;
}

double DatCoc::getTienCoc() const
{
    return tienCoc;
}

string DatCoc::getNgayDatCoc() const
{
    return ngayDatCoc;
}

string DatCoc::getTrangThai() const
{
    return trangThai;
}

void DatCoc::setTienCoc(double tienCoc)
{
    this->tienCoc = tienCoc;
}

void DatCoc::setTrangThai(string trangThai)
{
    this->trangThai = trangThai;
}

void DatCoc::hienThi() const
{
    cout << "Ma dat coc: " << maDatCoc << '\n';
    cout << "Ma dat ban: " << maDatBan << '\n';
    cout << "Tien coc: " << tienCoc << " VND\n";
    cout << "Ngay dat coc: " << ngayDatCoc << '\n';
    cout << "Trang thai: " << trangThai << '\n';
}
