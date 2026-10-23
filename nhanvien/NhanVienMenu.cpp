
#include "NhanVienMenu.h"
#include <iostream>

using namespace std;

void menuNhanVien(
    BanService& banService,
    DatBanService& datBanService,
    KhachHangService& khachHangService,
    MonAnService& monAnService,
    HoaDonService& hoaDonService
) {
    int chon;

    do {
        cout << "\n========== MENU NHAN VIEN ==========\n";
        cout << "1. Xem danh sach ban\n";
        cout << "2. Xem danh sach dat ban\n";
        cout << "3. Xem danh sach khach hang\n";
        cout << "4. Xem danh sach mon an\n";
        cout << "5. Xem danh sach hoa don\n";
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

        case 0:
            cout << "Dang xuat nhan vien!\n";
            break;

        default:
            cout << "Lua chon khong hop le!\n";
        }

    } while (chon != 0);
}
