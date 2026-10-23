
#include <QCoreApplication>
#include <QString>

#include <iostream>
#include <limits>
#include <string>

#include "DatabaseManager.h"
#include "admin/AdminMenu.h"
#include "nhanvien/NhanVienMenu.h"
#include "taikhoan/DangNhapService.h"
#include "taikhoan/PhanQuyen.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString loi;
    if (!DatabaseManager::connect(&loi)) {
        cerr << "KHONG KET NOI DUOC SQL SERVER:\n"
             << loi.toStdString() << '\n';
        return 1;
    }

    cout << "Da ket noi SQL Server thanh cong!\n";

    BanService banService;
    DatBanService datBanService;
    KhachHangService khachHangService;
    MonAnService monAnService;
    HoaDonService hoaDonService;
    TaiKhoanService taiKhoanService;

    DangNhapService dangNhapService(taiKhoanService);

    while (true) {
        cout << "\n====================================\n";
        cout << " UNG DUNG QUAN LY DAT BAN NHA HANG\n";
        cout << "====================================\n";
        cout << "1. Dang nhap\n";
        cout << "0. Thoat chuong trinh\n";
        cout << "Chon: ";

        int chon;

        if (!(cin >> chon)) {
            cin.clear();
            cin.ignore(
                numeric_limits<streamsize>::max(), '\n'
            );
            cout << "Vui long nhap so!\n";
            continue;
        }

        cin.ignore(
            numeric_limits<streamsize>::max(), '\n'
        );

        if (chon == 0)
            break;

        if (chon != 1) {
            cout << "Lua chon khong hop le!\n";
            continue;
        }

        string tenDangNhap;
        string matKhau;

        cout << "Ten dang nhap: ";
        getline(cin, tenDangNhap);

        cout << "Mat khau: ";
        getline(cin, matKhau);

        const TaiKhoan* tk =
            dangNhapService.dangNhap(
                tenDangNhap, matKhau
            );

        if (tk == nullptr)
            continue;

        // Sao chep tai khoan truoc khi vao menu.
        TaiKhoan taiKhoanDangNhap = *tk;

        if (PhanQuyen::laAdmin(taiKhoanDangNhap)) {
            menuAdmin(
                banService,
                datBanService,
                khachHangService,
                monAnService,
                hoaDonService,
                taiKhoanService
            );
        }
        else if (PhanQuyen::laNhanVien(taiKhoanDangNhap)) {
            menuNhanVien(
                banService,
                datBanService,
                khachHangService,
                monAnService,
                hoaDonService
            );
        }
        else {
            cout << "Tai khoan khong co quyen truy cap!\n";
        }
    }

    DatabaseManager::close();
    cout << "Da dong ket noi SQL Server.\n";

    return 0;
}
