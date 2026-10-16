
#include "PhanQuyen.h"

bool PhanQuyen::laAdmin(const TaiKhoan& tk)
{
    return tk.getVaiTro() == "Admin";
}

bool PhanQuyen::laNhanVien(const TaiKhoan& tk)
{
    return tk.getVaiTro() == "NhanVien";
}
