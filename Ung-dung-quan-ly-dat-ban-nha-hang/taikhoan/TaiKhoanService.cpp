
#include "TaiKhoanService.h"
using namespace std;

// Them tai khoan
bool TaiKhoanService::themTaiKhoan(const TaiKhoan& tk)
{
    if (!tk.hopLe())
    {
        cout << "Thong tin tai khoan khong hop le!\n";
        return false;
    }

    if (tk.getVaiTro() != "Admin" &&
        tk.getVaiTro() != "NhanVien")
    {
        cout << "Vai tro chi duoc la Admin hoac NhanVien!\n";
        return false;
    }

    if (timTheoMa(tk.getMaTK()) != nullptr)
    {
        cout << "Ma tai khoan da ton tai!\n";
        return false;
    }

    if (timTheoTenDangNhap(tk.getTenDangNhap()) != nullptr)
    {
        cout << "Ten dang nhap da ton tai!\n";
        return false;
    }

    danhSachTaiKhoan.push_back(tk);
    cout << "Them tai khoan thanh cong!\n";
    return true;
}

// Xoa tai khoan
bool TaiKhoanService::xoaTaiKhoan(const string& maTK)
{
    for (auto it = danhSachTaiKhoan.begin();
         it != danhSachTaiKhoan.end(); ++it)
    {
        if (it->getMaTK() == maTK)
        {
            danhSachTaiKhoan.erase(it);
            cout << "Xoa tai khoan thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay tai khoan!\n";
    return false;
}

// Doi mat khau
bool TaiKhoanService::doiMatKhau(
    const string& maTK,
    const string& matKhauCu,
    const string& matKhauMoi)
{
    if (matKhauMoi.empty())
    {
        cout << "Mat khau moi khong duoc de trong!\n";
        return false;
    }

    for (TaiKhoan& tk : danhSachTaiKhoan)
    {
        if (tk.getMaTK() == maTK)
        {
            if (tk.getMatKhau() != matKhauCu)
            {
                cout << "Mat khau cu khong dung!\n";
                return false;
            }

            tk.setMatKhau(matKhauMoi);
            cout << "Doi mat khau thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay tai khoan!\n";
    return false;
}

// Cap nhat vai tro
bool TaiKhoanService::capNhatVaiTro(
    const string& maTK,
    const string& vaiTroMoi)
{
    if (vaiTroMoi != "Admin" && vaiTroMoi != "NhanVien")
    {
        cout << "Vai tro khong hop le!\n";
        return false;
    }

    for (TaiKhoan& tk : danhSachTaiKhoan)
    {
        if (tk.getMaTK() == maTK)
        {
            tk.setVaiTro(vaiTroMoi);
            cout << "Cap nhat vai tro thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay tai khoan!\n";
    return false;
}

// Khoa hoac mo khoa tai khoan
bool TaiKhoanService::capNhatTrangThai(
    const string& maTK,
    const string& trangThaiMoi)
{
    if (trangThaiMoi != "HoatDong" &&
        trangThaiMoi != "BiKhoa")
    {
        cout << "Trang thai khong hop le!\n";
        return false;
    }

    for (TaiKhoan& tk : danhSachTaiKhoan)
    {
        if (tk.getMaTK() == maTK)
        {
            tk.setTrangThai(trangThaiMoi);
            cout << "Cap nhat trang thai thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay tai khoan!\n";
    return false;
}

// Tim theo ma tai khoan
const TaiKhoan* TaiKhoanService::timTheoMa(
    const string& maTK) const
{
    for (const TaiKhoan& tk : danhSachTaiKhoan)
    {
        if (tk.getMaTK() == maTK)
            return &tk;
    }

    return nullptr;
}

// Tim theo ten dang nhap
const TaiKhoan* TaiKhoanService::timTheoTenDangNhap(
    const string& tenDangNhap) const
{
    for (const TaiKhoan& tk : danhSachTaiKhoan)
    {
        if (tk.getTenDangNhap() == tenDangNhap)
            return &tk;
    }

    return nullptr;
}

// Hien thi danh sach tai khoan
void TaiKhoanService::hienThiDanhSach() const
{
    if (danhSachTaiKhoan.empty())
    {
        cout << "Danh sach tai khoan rong!\n";
        return;
    }

    cout << "\n========== DANH SACH TAI KHOAN ==========\n";

    for (const TaiKhoan& tk : danhSachTaiKhoan)
    {
        cout << "Ma TK: " << tk.getMaTK() << '\n';
        cout << "Ten dang nhap: " << tk.getTenDangNhap() << '\n';
        cout << "Vai tro: " << tk.getVaiTro() << '\n';
        cout << "Ma nhan vien: " << tk.getMaNV() << '\n';
        cout << "Trang thai: " << tk.getTrangThai() << '\n';
        cout << "-----------------------------------------\n";
    }
}

// Lay danh sach
const vector<TaiKhoan>&
TaiKhoanService::getDanhSachTaiKhoan() const
{
    return danhSachTaiKhoan;
}
