
#ifndef GOIMONSERVICE_H
#define GOIMONSERVICE_H

#include <iostream>
#include <string>
using namespace std;

#include "../services/MonAnService.h"
#include "../services/HoaDonService.h"

class GoiMonService
{
private:
    MonAnService& monAnService;
    HoaDonService& hoaDonService;

public:
    GoiMonService(
        MonAnService& monAn,
        HoaDonService& hoaDon
    );

    bool goiMon(
        const string& maHD,
        const string& maMon,
        int soLuong
    );

    bool suaSoLuong(
        const string& maHD,
        const string& maMon,
        int soLuongMoi
    );

    bool huyMon(
        const string& maHD,
        const string& maMon
    );
};

#endif
