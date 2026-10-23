#pragma once
#include <iostream>
#include <string>
#include "../oop/DatBan.h"
#include "../services/DatBanService.h"
using namespace std;

class ThongKeDatBanService
{
public:
    void thongKeTongQuan(const DatBanService& quanLyDatBan) const;
    void thongKeTheoNgay(
        const DatBanService& quanLyDatBan,
        const string& ngay) const;
    void thongKeKhungGio(
        const DatBanService& quanLyDatBan) const;
};