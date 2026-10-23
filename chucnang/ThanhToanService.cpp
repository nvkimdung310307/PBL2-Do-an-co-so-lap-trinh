#include "ThanhToanService.h"
#include <iostream>
#include <iomanip>
using namespace std;

ThanhToanService::ThanhToanService(HoaDonService& hoaDon)
    : hoaDonService(hoaDon)
{
}

// Thanh toan tien mat
bool ThanhToanService::thanhToanTienMat(const string& maHD)
{
    cout << "\n===== THANH TOAN TIEN MAT =====\n";

    return hoaDonService.thanhToan(maHD, "Tien mat");
}

// Thanh toan chuyen khoan
bool ThanhToanService::thanhToanChuyenKhoan(const string& maHD)
{
    cout << "\n===== THANH TOAN CHUYEN KHOAN =====\n";

    return hoaDonService.thanhToan(maHD, "Chuyen khoan");
}

// Thanh toan QR
bool ThanhToanService::thanhToanQR(const string& maHD)
{
    cout << "\n===== THANH TOAN QR =====\n";

    return hoaDonService.thanhToan(maHD, "QR");
}

// Chia tien theo so nguoi
bool ThanhToanService::chiaTien(
    const string& maHD,
    int soNguoi)
{
    if (soNguoi <= 0)
    {
        cout << "So nguoi chia tien phai lon hon 0!\n";
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
        cout << "Hoa don da thanh toan!\n";
        return false;
    }

    double tongTien = hd->getThanhTien();
    double tienMoiNguoi = tongTien / soNguoi;

    cout << fixed << setprecision(0);
    cout << "Tong tien: " << tongTien << " VND\n";
    cout << "So nguoi: " << soNguoi << '\n';
    cout << "Moi nguoi tra: " << tienMoiNguoi << " VND\n";

    return true;
}