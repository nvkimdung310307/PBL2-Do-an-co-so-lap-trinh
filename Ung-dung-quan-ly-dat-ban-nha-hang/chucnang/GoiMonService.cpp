
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

// Goi mon
bool GoiMonService::goiMon(
    const string& maHD,
    const string& maMon,
    int soLuong)
{
    if (soLuong <= 0)
    {
        cout << "So luong phai lon hon 0!\n";
        return false;
    }

    const HoaDon* hd = hoaDonService.timHoaDon(maHD);

    if (hd == nullptr)
    {
        cout << "Khong tim thay hoa don!\n";
        return false;
    }

    if (hd->getTrangThai() == "Da thanh toan")
    {
        cout << "Hoa don da thanh toan, khong the goi them mon!\n";
        return false;
    }

    const MonAn* mon = monAnService.timMonAn(maMon);

    if (mon == nullptr)
    {
        cout << "Khong tim thay mon an!\n";
        return false;
    }

    if (!mon->hopLe())
    {
        cout << "Thong tin mon an khong hop le!\n";
        return false;
    }

    // Them chi tiet hoa don bang ham cua HoaDonService.
    // Can doi ten ham theo HoaDonService.h hien tai cua ban.
    ChiTietHoaDon chiTiet(
        maHD,
        maMon,
        soLuong,
        mon->getDonGia()
    );

    cout << "Da kiem tra mon an va hoa don.\n";
    cout << "Mon: " << mon->getTenMon() << '\n';
    cout << "So luong: " << soLuong << '\n';
    cout << "Don gia: " << mon->getDonGia() << '\n';

    // Chua them chi tiet vao hoa don vi can xac nhan
    // dung ten ham themChiTiet trong HoaDonService.h.
    return false;
}

// Sua so luong mon
bool GoiMonService::suaSoLuong(
    const string& maHD,
    const string& maMon,
    int soLuongMoi)
{
    if (soLuongMoi <= 0)
    {
        cout << "So luong moi phai lon hon 0!\n";
        return false;
    }

    cout << "Can ket noi voi ham sua chi tiet hoa don.\n";
    return false;
}

// Huy mon
bool GoiMonService::huyMon(
    const string& maHD,
    const string& maMon)
{
    cout << "Can ket noi voi ham xoa chi tiet hoa don.\n";
    return false;
}
