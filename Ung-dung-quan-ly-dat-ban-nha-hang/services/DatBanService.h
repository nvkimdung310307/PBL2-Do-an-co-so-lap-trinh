
#ifndef DAT_BAN_SERVICE_H
#define DAT_BAN_SERVICE_H

#include <vector>
#include <string>
#include "../oop/DatBan.h"

using namespace std;

class DatBanService
{
private:
    vector<DatBan> danhSachDatBan;

    bool trungLich(
        const DatBan& datBan,
        const string& maDatBanBoQua = ""
    ) const;

public:
    // Them dat ban
    bool themDatBan(const DatBan& datBan);

    // Sua thong tin dat ban
    bool suaDatBan(
        const string& maDatBan,
        const DatBan& datBanMoi
    );

    // Huy dat ban
    bool huyDatBan(const string& maDatBan);
    // Xac nhan khach den
    bool xacNhanKhachDen(const string& maDatBan);

    // Danh dau khach khong den
    bool danhDauKhongDen(const string& maDatBan);

    // Xoa dat ban
    bool xoaDatBan(const string& maDatBan);

    // Tim theo ma dat ban
    const DatBan* timDatBan(
        const string& maDatBan
    ) const;

    // Tim theo ma khach hang
    void timTheoKhachHang(const string& maKH) const;

    // Tim theo ngay dat
    void timTheoNgay(const string& ngayDat) const;

    // Hien thi danh sach
    void hienThiDanhSach() const;

    // Lay danh sach dat ban
    const vector<DatBan>& getDanhSachDatBan() const;
};

#endif
