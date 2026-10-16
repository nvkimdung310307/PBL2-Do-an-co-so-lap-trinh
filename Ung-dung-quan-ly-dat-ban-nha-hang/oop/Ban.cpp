#include "Ban.h"

Ban::Ban()
{
    maBan = "";
    tenBan = "";
    soCho = 0;
    trangThai = "";
    maKhuVuc = "";
}

Ban::Ban(string maBan, string tenBan, int soCho, string trangThai, string maKhuVuc)
{
    this->maBan = maBan;
    this->tenBan = tenBan;
    this->soCho = soCho;
    this->trangThai = trangThai;
    this->maKhuVuc = maKhuVuc;
}

string Ban::getMaBan() const
{
    return maBan;
}

string Ban::getTenBan() const
{
    return tenBan;
}

int Ban::getSoCho() const
{
    return soCho;
}

string Ban::getTrangThai() const
{
    return trangThai;
}

string Ban::getMaKhuVuc() const
{
    return maKhuVuc;
}

void Ban::setMaBan(string maBan)
{
    this->maBan = maBan;
}

void Ban::setTenBan(string tenBan)
{
    this->tenBan = tenBan;
}

void Ban::setSoCho(int soCho)
{
    this->soCho = soCho;
}

void Ban::setTrangThai(string trangThai)
{
    this->trangThai = trangThai;
}

void Ban::setMaKhuVuc(string maKhuVuc)
{
    this->maKhuVuc = maKhuVuc;
}

bool Ban::hopLe() const
{
    return !maBan.empty() && !tenBan.empty() && soCho > 0 && !maKhuVuc.empty();
}