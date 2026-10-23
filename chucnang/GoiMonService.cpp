
#include "GoiMonService.h"
#include "../oop/MonAn.h"
#include "../oop/HoaDon.h"
#include "../oop/ChiTietHoaDon.h"

#include <iostream>
using namespace std;

GoiMonService::GoiMonService(
    MonAnService& monAn,
    HoaDonService& hoaDon)
    : monAnService(monAn),
      hoaDonService(hoaDon)
{
}

bool GoiMonService::goiMon(
    const string& maHD,
    const string& maMon,
    int soLuong)
{
    if (soLuong <= 0) {
        cout << "So luong phai lon hon 0!\n";
        return false;
    }

    const HoaDon* hd = hoaDonService.timHoaDon(maHD);

    if (hd == nullptr) {
        cout << "Khong tim thay hoa don!\n";
        return false;
    }

    if (hd->getTrangThai() == "Da thanh toan" ||
        hd->getTrangThai() == "Đã thanh toán") {
        cout << "Hoa don da thanh toan!\n";
        return false;
    }

    const MonAn* mon = monAnService.timMonAn(maMon);

    if (mon == nullptr) {
        cout << "Khong tim thay mon an!\n";
        return false;
    }

    if (!mon->hopLe()) {
        cout << "Thong tin mon an khong hop le!\n";
        return false;
    }

    if (mon->getTrangThai() != "Dang ban" &&
        mon->getTrangThai() != "Đang bán" &&
        mon->getTrangThai() != "Con hang" &&
        mon->getTrangThai() != "Còn hàng" &&
        mon->getTrangThai() != "HoatDong" &&
        mon->getTrangThai() != "Hoạt động") {
        cout << "Mon an hien khong phuc vu!\n";
        return false;
    }

    ChiTietHoaDon ct(
        maHD,
        maMon,
        soLuong,
        mon->getDonGia()
    );

    return hoaDonService.themChiTiet(ct);
}

bool GoiMonService::suaSoLuong(
    const string& maHD,
    const string& maMon,
    int soLuongMoi)
{
    return hoaDonService.suaChiTiet(
        maHD,
        maMon,
        soLuongMoi
    );
}

bool GoiMonService::huyMon(
    const string& maHD,
    const string& maMon)
{
    return hoaDonService.xoaChiTiet(maHD, maMon);
}
