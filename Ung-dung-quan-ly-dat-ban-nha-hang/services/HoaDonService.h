
#ifndef HOA_DON_SERVICE_H
#define HOA_DON_SERVICE_H

#include <vector>
#include <string>
#include "../oop/HoaDon.h"
#include "../oop/ChiTietHoaDon.h"

using namespace std;

class HoaDonService
{
private:
    vector<HoaDon> danhSachHoaDon;
    vector<ChiTietHoaDon> danhSachChiTiet;

public:
    // Tao hoa don
    bool taoHoaDon(const HoaDon& hd);

    // Them mon vao hoa don
    bool themChiTiet(const ChiTietHoaDon& ct);

    // Xoa mon khoi hoa don
    bool xoaChiTiet(
        const string& maHD,
        const string& maMon
    );

    // Tinh tong tien va thanh tien
    bool tinhTien(const string& maHD, double tienGiam);

    // Thanh toan hoa don
    bool thanhToan(
        const string& maHD,
        const string& phuongThuc
    );

    // Tim hoa don
    const HoaDon* timHoaDon(const string& maHD) const;

    // Hien thi danh sach hoa don
    void hienThiDanhSach() const;

    // Hien thi chi tiet mot hoa don
    void hienThiChiTiet(const string& maHD) const;

    // Lay danh sach hoa don
    const vector<HoaDon>& getDanhSachHoaDon() const;

    // Lay danh sach chi tiet
    const vector<ChiTietHoaDon>& getDanhSachChiTiet() const;
};

#endif
