#include "QuanLyBan.h"
#include <iostream>
#include <windows.h>

using namespace std;

void QuanLyBan::taoBanMacDinh() {

    // Tao san 10 ban
    for (int i = 1; i <= 10; i++) {

        Ban b(i, 4);

        ds.push_back(b);
    }
}
void QuanLyBan::hienThiBan() {

    system("cls");

    cout << "\n";
    cout << "============================================\n";
    cout << "          QUAN LY BAN NHA HANG\n";
    cout << "============================================\n\n";

    for (int i = 0; i < ds.size(); i++) {

        ds[i].hienThi();

        // 2 ban tren 1 hang
        if ((i + 1) % 2 == 0)
            cout << "\n";
        else
            cout << "        ";
    }

    cout << "\n";
    cout << "============================================\n";

    // Mau chu thich
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(h, 10);
    cout << "XANH = BAN TRONG     ";

    SetConsoleTextAttribute(h, 12);
    cout << "DO = BAN CO KHACH";

    SetConsoleTextAttribute(h, 7);

    cout << "\n============================================\n";
}
int QuanLyBan::timViTri(int ma) {

    for (int i = 0; i < ds.size(); i++) {

        if (ds[i].getMaBan() == ma)
            return i;
    }

    return -1;
}
void QuanLyBan::chonBan() {

    hienThiBan();

    int ma;

    cout << "\nNhap so ban muon chon: ";
    cin >> ma;

    int vt = timViTri(ma);

    if (vt == -1) {

        cout << "Khong co ban nay!\n";
        system("pause");
        return;
    }

    cout << "\nBan " << ma << " da duoc chon!\n";

    if (ds[vt].getCoKhach()) {

        cout << "Trang thai: DANG CO KHACH\n";

    } else {

        cout << "Trang thai: DANG TRONG\n";
    }

    system("pause");
}
void QuanLyBan::nhanKhach() {

    hienThiBan();

    int ma;

    cout << "\nChon ban de nhan khach: ";
    cin >> ma;

    int vt = timViTri(ma);

    if (vt == -1) {

        cout << "Khong co ban nay!\n";
        system("pause");
        return;
    }

    if (ds[vt].getCoKhach()) {

        cout << "Ban nay dang co khach!\n";
        system("pause");
        return;
    }

    ds[vt].nhanKhach();

    cout << "\nNhan khach vao BAN " << ma << " thanh cong!\n";

    system("pause");
}
void QuanLyBan::thanhToan() {

    hienThiBan();

    int ma;

    cout << "\nChon ban thanh toan: ";
    cin >> ma;

    int vt = timViTri(ma);

    if (vt == -1) {

        cout << "Khong co ban nay!\n";
        system("pause");
        return;
    }

    if (!ds[vt].getCoKhach()) {

        cout << "Ban nay dang trong!\n";
        system("pause");
        return;
    }

    ds[vt].thanhToan();

    cout << "\nThanh toan BAN " << ma << " thanh cong!\n";
    cout << "Ban da tro lai trang thai TRONG.\n";

    system("pause");
}
void QuanLyBan::themBan() {

    int ma;
    int cho;

    cout << "\nNhap ma ban moi: ";
    cin >> ma;

    if (timViTri(ma) != -1) {

        cout << "Ma ban da ton tai!\n";
        system("pause");
        return;
    }

    cout << "Nhap so cho: ";
    cin >> cho;

    if (cho <= 0) {

        cout << "So cho khong hop le!\n";
        system("pause");
        return;
    }

    Ban b(ma, cho);

    ds.push_back(b);

    cout << "\nThem ban thanh cong!\n";

    system("pause");
}
void QuanLyBan::xoaBan() {

    hienThiBan();

    int ma;

    cout << "\nNhap ban can xoa: ";
    cin >> ma;

    int vt = timViTri(ma);

    if (vt == -1) {

        cout << "Khong co ban nay!\n";
        system("pause");
        return;
    }

    if (ds[vt].getCoKhach()) {

        cout << "Khong the xoa ban dang co khach!\n";
        system("pause");
        return;
    }

    ds.erase(ds.begin() + vt);

    cout << "Xoa ban thanh cong!\n";

    system("pause");
}
void QuanLyBan::menuQuanLy() {

    int chon;

    do {

        system("cls");

        cout << "============================================\n";
        cout << "             QUAN LY BAN\n";
        cout << "============================================\n";

        cout << "1. Hien thi ban\n";
        cout << "2. Chon ban\n";
        cout << "3. Nhan khach\n";
        cout << "4. Thanh toan\n";
        cout << "5. Them ban\n";
        cout << "6. Xoa ban\n";
        cout << "0. Quay lai\n";

        cout << "============================================\n";

        cout << "Nhap lua chon: ";
        cin >> chon;

        switch (chon) {

            case 1:
                hienThiBan();
                system("pause");
                break;

            case 2:
                chonBan();
                break;

            case 3:
                nhanKhach();
                break;

            case 4:
                thanhToan();
                break;

            case 5:
                themBan();
                break;

            case 6:
                xoaBan();
                break;

            case 0:
                cout << "Quay lai...\n";
                break;

            default:
                cout << "Lua chon khong hop le!\n";
                system("pause");
        }

    } while (chon != 0);
}
