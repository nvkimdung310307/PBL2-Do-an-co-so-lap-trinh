#ifndef BAN_H
#define BAN_H

#include <iostream>
#include <string>
using namespace std;

class Ban
{
private:
    string maBan;
    string tenBan;
    int soCho;
    string trangThai;
    string maKhuVuc;

public:
    Ban();
    Ban(string maBan, string tenBan, int soCho, string trangThai, string maKhuVuc);

    string getMaBan() const;
    string getTenBan() const;
    int getSoCho() const;
    string getTrangThai() const;
    string getMaKhuVuc() const;

    void setMaBan(string maBan);
    void setTenBan(string tenBan);
    void setSoCho(int soCho);
    void setTrangThai(string trangThai);
    void setMaKhuVuc(string maKhuVuc);

    bool hopLe() const;
};

#endif