
#ifndef KHACH_HANG_SERVICE_H
#define KHACH_HANG_SERVICE_H

#include <vector>
#include <string>
#include "../oop/KhachHang.h"

using namespace std;

class KhachHangService
{
private:
    vector<KhachHang> danhSachKhachHang;

public:
    // Them khach hang
    bool themKhachHang(const KhachHang& kh);

    // Sua thong tin khach hang
    bool suaKhachHang(
        const string& maKH,
        const KhachHang& khMoi
    );

    // Xoa khach hang
    bool xoaKhachHang(const string& maKH);

    // Tim khach hang theo ma
    const KhachHang* timKhachHang(
        const string& maKH
    ) const;

    // Tim khach hang theo so dien thoai
    const KhachHang* timTheoSoDienThoai(
        const string& soDienThoai
    ) const;

    // Hien thi danh sach
    void hienThiDanhSach() const;

    // Lay danh sach khach hang
    const vector<KhachHang>& getDanhSachKhachHang() const;
};

#endif
