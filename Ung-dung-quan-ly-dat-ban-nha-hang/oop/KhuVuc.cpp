#include "KhuVuc.h"

KhuVuc::KhuVuc()
{
    maKhuVuc = "";
    tenKhuVuc = "";
    moTa = "";
}

KhuVuc::KhuVuc(string maKhuVuc, string tenKhuVuc, string moTa)
{
    this->maKhuVuc = maKhuVuc;
    this->tenKhuVuc = tenKhuVuc;
    this->moTa = moTa;
}

string KhuVuc::getMaKhuVuc() const
{
    return maKhuVuc;
}

string KhuVuc::getTenKhuVuc() const
{
    return tenKhuVuc;
}

string KhuVuc::getMoTa() const
{
    return moTa;
}

void KhuVuc::setMaKhuVuc(string maKhuVuc)
{
    this->maKhuVuc = maKhuVuc;
}

void KhuVuc::setTenKhuVuc(string tenKhuVuc)
{
    this->tenKhuVuc = tenKhuVuc;
}

void KhuVuc::setMoTa(string moTa)
{
    this->moTa = moTa;
}

bool KhuVuc::hopLe() const
{
    return !maKhuVuc.empty() && !tenKhuVuc.empty();
}