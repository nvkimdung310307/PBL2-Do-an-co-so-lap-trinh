
#include "DangNhapService.h"
#include <iostream>
using namespace std;

DangNhapService::DangNhapService(TaiKhoanService& service)
    : taiKhoanService(service)
{
}

const TaiKhoan* DangNhapService::dangNhap(
    const string& tenDangNhap,
    const string& matKhau) const
{
    const TaiKhoan* tk =
        taiKhoanService.timTheoTenDangNhap(tenDangNhap);

    if (tk == nullptr)
    {
        cout << "Ten dang nhap khong ton tai!\n";
        return nullptr;
    }

    if (tk->getMatKhau() != matKhau)
    {
        cout << "Mat khau khong dung!\n";
        return nullptr;
    }

    if (tk->getTrangThai() != "HoatDong")
    {
        cout << "Tai khoan dang bi khoa!\n";
        return nullptr;
    }

    if (tk->getVaiTro() != "Admin" &&
        tk->getVaiTro() != "NhanVien")
    {
        cout << "Vai tro tai khoan khong hop le!\n";
        return nullptr;
    }

    cout << "Dang nhap thanh cong!\n";
    return tk;
}
