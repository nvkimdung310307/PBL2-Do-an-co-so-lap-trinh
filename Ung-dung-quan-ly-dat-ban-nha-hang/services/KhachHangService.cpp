
#include "KhachHangService.h"
#include <iostream>

using namespace std;

// 1. Them khach hang
bool KhachHangService::themKhachHang(
    const KhachHang& kh)
{
    if (!kh.hopLe())
    {
        cout << "Thong tin khach hang khong hop le!\n";
        return false;
    }

    if (timKhachHang(kh.getMaKH()) != nullptr)
    {
        cout << "Ma khach hang da ton tai!\n";
        return false;
    }

    if (timTheoSoDienThoai(kh.getSoDienThoai()) != nullptr)
    {
        cout << "So dien thoai da ton tai!\n";
        return false;
    }

    danhSachKhachHang.push_back(kh);

    cout << "Them khach hang thanh cong!\n";
    return true;
}

// 2. Sua thong tin khach hang
bool KhachHangService::suaKhachHang(
    const string& maKH,
    const KhachHang& khMoi)
{
    for (size_t i = 0; i < danhSachKhachHang.size(); i++)
    {
        if (danhSachKhachHang[i].getMaKH() == maKH)
        {
            // Khong cho doi ma khach hang
            KhachHang khCapNhat(
                maKH,
                khMoi.getHoTen(),
                khMoi.getSoDienThoai(),
                khMoi.getEmail(),
                khMoi.getDiaChi()
            );

            if (!khCapNhat.hopLe())
            {
                cout << "Thong tin moi khong hop le!\n";
                return false;
            }

            // Kiem tra trung so dien thoai voi khach khac
            const KhachHang* khTimDuoc =
                timTheoSoDienThoai(khMoi.getSoDienThoai());

            if (khTimDuoc != nullptr &&
                khTimDuoc->getMaKH() != maKH)
            {
                cout << "So dien thoai da thuoc khach hang khac!\n";
                return false;
            }

            danhSachKhachHang[i] = khCapNhat;

            cout << "Sua khach hang thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay khach hang can sua!\n";
    return false;
}

// 3. Xoa khach hang
bool KhachHangService::xoaKhachHang(
    const string& maKH)
{
    for (size_t i = 0; i < danhSachKhachHang.size(); i++)
    {
        if (danhSachKhachHang[i].getMaKH() == maKH)
        {
            danhSachKhachHang.erase(
                danhSachKhachHang.begin() + i
            );

            cout << "Xoa khach hang thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay khach hang can xoa!\n";
    return false;
}

// 4. Tim khach hang theo ma
const KhachHang* KhachHangService::timKhachHang(
    const string& maKH) const
{
    for (size_t i = 0; i < danhSachKhachHang.size(); i++)
    {
        if (danhSachKhachHang[i].getMaKH() == maKH)
        {
            return &danhSachKhachHang[i];
        }
    }

    return nullptr;
}

// 5. Tim theo so dien thoai
const KhachHang* KhachHangService::timTheoSoDienThoai(
    const string& soDienThoai) const
{
    for (size_t i = 0; i < danhSachKhachHang.size(); i++)
    {
        if (danhSachKhachHang[i].getSoDienThoai()
            == soDienThoai)
        {
            return &danhSachKhachHang[i];
        }
    }

    return nullptr;
}

// 6. Hien thi danh sach
void KhachHangService::hienThiDanhSach() const
{
    if (danhSachKhachHang.empty())
    {
        cout << "Danh sach khach hang dang rong!\n";
        return;
    }

    cout << "\n======= DANH SACH KHACH HANG =======\n";

    for (size_t i = 0; i < danhSachKhachHang.size(); i++)
    {
        cout << "Ma KH: "
             << danhSachKhachHang[i].getMaKH() << '\n';

        cout << "Ho ten: "
             << danhSachKhachHang[i].getHoTen() << '\n';

        cout << "So dien thoai: "
             << danhSachKhachHang[i].getSoDienThoai() << '\n';

        cout << "Email: "
             << danhSachKhachHang[i].getEmail() << '\n';

        cout << "Dia chi: "
             << danhSachKhachHang[i].getDiaChi() << '\n';

        cout << "------------------------------------\n";
    }
}

// 7. Lay danh sach khach hang
const vector<KhachHang>&
KhachHangService::getDanhSachKhachHang() const
{
    return danhSachKhachHang;
}
