#include "KhachChoService.h"

void KhachChoService::themKhachCho(const KhachCho& khach)
{
    if (khach.getMaKhachCho().empty() ||
        khach.getTenKhach().empty() ||
        khach.getSoDienThoai().empty() ||
        khach.getSoLuongKhach() <= 0)
    {
        cout << "Thong tin khach cho khong hop le!\n";
        return;
    }

    for (size_t i = 0; i < danhSachKhachCho.size(); i++)
    {
        if (danhSachKhachCho[i].getMaKhachCho()
            == khach.getMaKhachCho())
        {
            cout << "Ma khach cho da ton tai!\n";
            return;
        }
    }

    danhSachKhachCho.push_back(khach);
    cout << "Them khach vao danh sach cho thanh cong!\n";
}

void KhachChoService::hienThiDanhSach() const
{
    if (danhSachKhachCho.empty())
    {
        cout << "Danh sach khach cho rong!\n";
        return;
    }

    for (size_t i = 0; i < danhSachKhachCho.size(); i++)
    {
        cout << "\n--- Khach cho thu " << i + 1 << " ---\n";
        danhSachKhachCho[i].hienThi();
    }
}

bool KhachChoService::goiKhach(const string& maKhachCho)
{
    for (size_t i = 0; i < danhSachKhachCho.size(); i++)
    {
        if (danhSachKhachCho[i].getMaKhachCho() == maKhachCho)
        {
            if (danhSachKhachCho[i].getTrangThai() != "DangCho")
            {
                cout << "Khach khong con trong danh sach cho!\n";
                return false;
            }

            danhSachKhachCho[i].setTrangThai("DaGoi");
            cout << "Da goi khach thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay khach cho!\n";
    return false;
}

bool KhachChoService::huyKhachCho(const string& maKhachCho)
{
    for (size_t i = 0; i < danhSachKhachCho.size(); i++)
    {
        if (danhSachKhachCho[i].getMaKhachCho() == maKhachCho)
        {
            if (danhSachKhachCho[i].getTrangThai() != "DangCho")
            {
                cout << "Khong the huy: khach khong con dang cho!\n";
                return false;
            }

            danhSachKhachCho[i].setTrangThai("DaHuy");
            cout << "Da huy khach cho!\n";
            return true;
        }
    }

    cout << "Khong tim thay khach cho!\n";
    return false;
}
