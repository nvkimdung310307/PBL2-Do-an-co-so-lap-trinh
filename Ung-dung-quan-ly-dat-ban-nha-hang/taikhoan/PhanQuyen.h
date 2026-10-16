#ifndef PHANQUYEN_H
#define PHANQUYEN_H

#include "TaiKhoan.h"
using namespace std;

class PhanQuyen
{
public:
    static bool laAdmin(const TaiKhoan& tk);
    static bool laNhanVien(const TaiKhoan& tk);
};

#endif
