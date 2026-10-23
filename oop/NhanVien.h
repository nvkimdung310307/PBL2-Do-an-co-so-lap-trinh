#ifndef NHANVIEN_H
#define NHANVIEN_H

#include <iostream>
#include <string>
using namespace std;

class NhanVien
{
private:
    string maNV;
    string hoTen;
    string gioiTinh;
    string ngaySinh;
    string soDienThoai;
    string email;
    string chucVu;
    double luong;
    string trangThai;

public:
    NhanVien();
    NhanVien(string maNV, string hoTen, string gioiTinh, string ngaySinh, string soDienThoai, string email, string chucVu, double luong, string trangThai);

    string getMaNV() const;
    string getHoTen() const;
    string getGioiTinh() const;
    string getNgaySinh() const;
    string getSoDienThoai() const;
    string getEmail() const;
    string getChucVu() const;
    double getLuong() const;
    string getTrangThai() const;

    void setMaNV(string maNV);
    void setHoTen(string hoTen);
    void setGioiTinh(string gioiTinh);
    void setNgaySinh(string ngaySinh);
    void setSoDienThoai(string soDienThoai);
    void setEmail(string email);
    void setChucVu(string chucVu);
    void setLuong(double luong);
    void setTrangThai(string trangThai);

    bool hopLe() const;
};

#endif