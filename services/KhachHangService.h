#ifndef KHACH_HANG_SERVICE_H
#define KHACH_HANG_SERVICE_H

#include <vector>
#include <string>
#include "../oop/KhachHang.h"

using namespace std;

class KhachHangService
{
private:
    mutable vector<KhachHang> danhSachKhachHang;
    bool napDanhSachSQL() const;

public:
    bool themKhachHang(const KhachHang& kh);
    bool suaKhachHang(const string& maKH, const KhachHang& khMoi);
    bool xoaKhachHang(const string& maKH);
    const KhachHang* timKhachHang(const string& maKH) const;
    const KhachHang* timTheoSoDienThoai(const string& soDienThoai) const;
    void hienThiDanhSach() const;
    const vector<KhachHang>& getDanhSachKhachHang() const;
};

#endif
