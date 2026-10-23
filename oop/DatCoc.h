#pragma once
#include <iostream>
#include <string>
using namespace std;

class DatCoc
{
private:
    string maDatCoc;
    string maDatBan;
    double tienCoc;
    string ngayDatCoc;
    string trangThai;

public:
    DatCoc();
    DatCoc(string maCoc, string maDatBan,
           double tienCoc, string ngayDatCoc);

    string getMaDatCoc() const;
    string getMaDatBan() const;
    double getTienCoc() const;
    string getNgayDatCoc() const;
    string getTrangThai() const;

    void setTienCoc(double tienCoc);
    void setTrangThai(string trangThai);
    void hienThi() const;
};
