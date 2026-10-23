
#include "AdminMenu.h"
#include <iostream>

using namespace std;

void menuAdmin(
    BanService& banService,
    DatBanService& datBanService,
    KhachHangService& khachHangService,
    MonAnService& monAnService,
    HoaDonService& hoaDonService,
    TaiKhoanService& taiKhoanService
) {
    int chon;

    do {
        cout << "\n========== MENU QUAN TRI ==========\n";
        cout << "1. Quan ly ban\n";
        cout << "2. Quan ly dat ban\n";
        cout << "3. Quan ly khach hang\n";
        cout << "4. Quan ly mon an\n";
        cout << "5. Quan ly hoa don\n";
        cout << "6. Quan ly tai khoan\n";
        cout << "0. Dang xuat\n";
        cout << "Chon chuc nang: ";
        cin >> chon;

        switch (chon) {
        case 1:
            banService.hienThiDanhSach();
            break;

        case 2:
            datBanService.hienThiDanhSach();
            break;

        case 3:
            khachHangService.hienThiDanhSach();
            break;

        case 4:
            monAnService.hienThiDanhSach();
            break;

        case 5:
            hoaDonService.hienThiDanhSach();
            break;

        case 6:
            taiKhoanService.hienThiDanhSach();
            break;

        case 0:
            cout << "Dang xuat quan tri!\n";
            break;

        default:
            cout << "Lua chon khong hop le!\n";
        }

    } while (chon != 0);
}
