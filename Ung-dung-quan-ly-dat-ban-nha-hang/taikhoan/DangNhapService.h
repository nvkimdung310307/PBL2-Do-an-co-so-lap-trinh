
#ifndef DANGNHAPSERVICE_H
#define DANGNHAPSERVICE_H

#include "TaiKhoanService.h"
#include "TaiKhoan.h"
#include <string>
using namespace std;

class DangNhapService
{
private:
    TaiKhoanService& taiKhoanService;

public:
    DangNhapService(TaiKhoanService& service);

    const TaiKhoan* dangNhap(
        const string& tenDangNhap,
        const string& matKhau
    ) const;
};

#endif
