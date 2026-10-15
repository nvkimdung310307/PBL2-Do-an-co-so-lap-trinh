#ifndef DATBAN_H
#define DATBAN_H

#include <iostream>
#include <string>
using namespace std;

class DatBan
{
private:
    string maDatBan;
    string maKH;
    string maBan;
    string maNV;
    string ngayDat;
    string gioDat;
    int soNguoi;
    string trangThai;
    string ghiChu;

public:
    DatBan();
    DatBan(string maDatBan, string maKH, string maBan, string maNV, string ngayDat, string gioDat, int soNguoi, string trangThai, string ghiChu);

    string getMaDatBan() const;
    string getMaKH() const;
    string getMaBan() const;
    string getMaNV() const;
    string getNgayDat() const;
    string getGioDat() const;
    int getSoNguoi() const;
    string getTrangThai() const;
    string getGhiChu() const;

    void setMaDatBan(string maDatBan);
    void setMaKH(string maKH);
    void setMaBan(string maBan);
    void setMaNV(string maNV);
    void setNgayDat(string ngayDat);
    void setGioDat(string gioDat);
    void setSoNguoi(int soNguoi);
    void setTrangThai(string trangThai);
    void setGhiChu(string ghiChu);

    bool hopLe() const;
};

#endif
