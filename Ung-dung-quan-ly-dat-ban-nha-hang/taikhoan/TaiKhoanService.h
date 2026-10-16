
#ifndef TAIKHOANSERVICE_H
#define TAIKHOANSERVICE_H

#include "TaiKhoan.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;

class TaiKhoanService
{
private:
    vector<TaiKhoan> danhSachTaiKhoan;

public:
    bool themTaiKhoan(const TaiKhoan& tk);
    bool xoaTaiKhoan(const string& maTK);
    bool doiMatKhau(
        const string& maTK,
        const string& matKhauCu,
        const string& matKhauMoi
    );

    bool capNhatVaiTro(
        const string& maTK,
        const string& vaiTroMoi
    );

    bool capNhatTrangThai(
        const string& maTK,
        const string& trangThaiMoi
    );

    const TaiKhoan* timTheoMa(const string& maTK) const;
    const TaiKhoan* timTheoTenDangNhap(
        const string& tenDangNhap
    ) const;

    void hienThiDanhSach() const;

    const vector<TaiKhoan>& getDanhSachTaiKhoan() const;
};

#endif
