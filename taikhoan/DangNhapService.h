
#ifndef DANGNHAPSERVICE_H
#define DANGNHAPSERVICE_H

#include "TaiKhoanService.h"
#include "../oop/TaiKhoan.h"

#include <string>

class DangNhapService
{
private:
    TaiKhoanService& taiKhoanService;
    TaiKhoan taiKhoanDangNhap;

public:
    explicit DangNhapService(TaiKhoanService& service);

    const TaiKhoan* dangNhap(
        const std::string& tenDangNhap,
        const std::string& matKhau
    );
};

#endif
