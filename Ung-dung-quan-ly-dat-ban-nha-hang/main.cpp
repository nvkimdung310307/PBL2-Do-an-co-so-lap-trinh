
#include <iostream>
#include <limits>

#include "admin/AdminMenu.h"
#include "nhanvien/NhanVienMenu.h"

using namespace std;

int main() {
    BanService banService;
    DatBanService datBanService;
    KhachHangService khachHangService;
    MonAnService monAnService;
    HoaDonService hoaDonService;
    TaiKhoanService taiKhoanService;

    int chon;

    do {
        cout << "\n====================================\n";
        cout << " UNG DUNG QUAN LY DAT BAN NHA HANG\n";
        cout << "====================================\n";
        cout << "1. Quan tri vien\n";
        cout << "2. Nhan vien\n";
        cout << "0. Thoat chuong trinh\n";
        cout << "Chon vai tro: ";

        if (!(cin >> chon)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Vui long nhap so!\n";
            continue;
        }

        switch (chon) {
        case 1:
            menuAdmin(
                banService,
                datBanService,
                khachHangService,
                monAnService,
                hoaDonService,
                taiKhoanService
            );
            break;

        case 2:
            menuNhanVien(
                banService,
                datBanService,
                khachHangService,
                monAnService,
                hoaDonService
            );
            break;

        case 0:
            cout << "Ket thuc chuong trinh!\n";
            break;

        default:
            cout << "Lua chon khong hop le!\n";
        }

    } while (chon != 0);

    return 0;
}
