
#ifndef KHUVUC_H
#define KHUVUC_H

#include <iostream>
#include <string>
using namespace std;

class KhuVuc
{
private:
    string maKhuVuc;
    string tenKhuVuc;
    string moTa;

public:
    KhuVuc();
    KhuVuc(string maKhuVuc, string tenKhuVuc, string moTa);

    string getMaKhuVuc() const;
    string getTenKhuVuc() const;
    string getMoTa() const;

    void setMaKhuVuc(string maKhuVuc);
    void setTenKhuVuc(string tenKhuVuc);
    void setMoTa(string moTa);

    bool hopLe() const;
};

#endif