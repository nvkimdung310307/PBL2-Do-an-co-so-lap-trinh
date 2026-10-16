#ifndef THANHTOANSERVICE_H
#define THANHTOANSERVICE_H

#include <iostream>
#include <string>
using namespace std;

#include "../services/HoaDonService.h"

class ThanhToanService
{
private:
    HoaDonService& hoaDonService;

public:
    ThanhToanService(HoaDonService& hoaDon);

    bool thanhToanTienMat(const string& maHD);

    bool thanhToanChuyenKhoan(const string& maHD);

    bool thanhToanQR(const string& maHD);

    bool chiaTien(
        const string& maHD,
        int soNguoi
    );
};

#endif