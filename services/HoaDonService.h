
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
    mutable vector<HoaDon> danhSachHoaDon;
    mutable vector<ChiTietHoaDon> danhSachChiTiet;
    bool napDanhSachSQL() const;

public:
    // Tao hoa don
    bool taoHoaDon(const HoaDon& hd);

    // Them mon vao hoa don
    bool themChiTiet(const ChiTietHoaDon& ct);
bool suaChiTiet(
    const string& maHD,
    const string& maMon,
    int soLuongMoi
);
    // Xoa mon khoi hoa don
    bool xoaChiTiet(
        const string& maHD,
        const string& maMon
    );

    // Tinh tong tien va thanh tien
    bool tinhTien(const string& maHD, double tienGiam);

    // Thanh toan hoa don
    // Gop hoa don nguon vao hoa don dich (ca hai phai chua thanh toan).
    bool gopHoaDon(const string& maHDNguon, const string& maHDDich);

    // Tach so luong mon tu hoa don nguon sang hoa don dich dang chua thanh toan.
    bool tachHoaDon(const string& maHDNguon, const string& maHDDich,
                    const string& maMon, int soLuong);

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
