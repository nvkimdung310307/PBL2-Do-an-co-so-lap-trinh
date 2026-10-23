#ifndef TAIKHOANSERVICE_H
#define TAIKHOANSERVICE_H

#include "../oop/TaiKhoan.h"
#include <string>
#include <vector>

class TaiKhoanService
{
private:
    mutable std::vector<TaiKhoan> danhSachTaiKhoan;
    bool napDanhSachSQL() const;
    bool taiKhoanTonTai(const std::string& maTK) const;

public:
    bool themTaiKhoan(const TaiKhoan& tk);
    bool xoaTaiKhoan(const std::string& maTK);
    bool doiMatKhau(const std::string& maTK,
                    const std::string& matKhauCu,
                    const std::string& matKhauMoi);
    bool capNhatVaiTro(const std::string& maTK,
                       const std::string& vaiTroMoi);
    bool capNhatTrangThai(const std::string& maTK,
                          const std::string& trangThaiMoi);

    const TaiKhoan* timTheoMa(const std::string& maTK) const;
    const TaiKhoan* timTheoTenDangNhap(const std::string& tenDangNhap) const;
    void hienThiDanhSach() const;
    const std::vector<TaiKhoan>& getDanhSachTaiKhoan() const;
};

#endif
