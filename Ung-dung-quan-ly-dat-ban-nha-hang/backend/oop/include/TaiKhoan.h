#ifndef TAIKHOAN_H
#define TAIKHOAN_H

#include <iostream>
#include <string>
using namespace std;

class TaiKhoan
{
private:
    string maTK;
    string tenDangNhap;
    string matKhau;
    string vaiTro;
    string maNV;
    string trangThai;

public:
    TaiKhoan();
    TaiKhoan(string maTK, string tenDangNhap, string matKhau, string vaiTro, string maNV, string trangThai);

    string getMaTK() const;
    string getTenDangNhap() const;
    string getMatKhau() const;
    string getVaiTro() const;
    string getMaNV() const;
    string getTrangThai() const;

    void setMaTK(string maTK);
    void setTenDangNhap(string tenDangNhap);
    void setMatKhau(string matKhau);
    void setVaiTro(string vaiTro);
    void setMaNV(string maNV);
    void setTrangThai(string trangThai);

    bool hopLe() const;
};

#endif
