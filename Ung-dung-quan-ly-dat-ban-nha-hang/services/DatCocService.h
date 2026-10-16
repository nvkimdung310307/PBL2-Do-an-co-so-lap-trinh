#pragma once
#include <iostream>
#include <vector>
#include "../oop/DatCoc.h"
using namespace std;

class DatCocService
{
private:
    vector<DatCoc> danhSachDatCoc;

public:
    bool themDatCoc(const DatCoc& datCoc);
    bool hoanCoc(const string& maDatCoc);
    bool capNhatTienCoc(const string& maDatCoc,
                        double tienMoi);
    void hienThiDanhSach() const;
    double tinhTongTienCoc() const;
    double tinhTienCoc(const string& maDatBan) const;
};
