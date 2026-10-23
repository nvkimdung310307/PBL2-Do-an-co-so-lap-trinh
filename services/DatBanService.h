#ifndef DAT_BAN_SERVICE_H
#define DAT_BAN_SERVICE_H

#include <vector>
#include <string>
#include "../oop/DatBan.h"

using namespace std;

class DatBanService
{
private:
    mutable vector<DatBan> danhSachDatBan;
    bool napDanhSachSQL() const;

public:
    bool themDatBan(const DatBan& datBan);
    bool suaDatBan(const string& maDatBan, const DatBan& datBanMoi);
    bool chuyenBanSQL(const string& maDatBan, const string& maBanMoi);
    bool huyDatBan(const string& maDatBan);
    bool xacNhanKhachDen(const string& maDatBan);
    bool hoanThanhDatBan(const string& maDatBan);
    bool danhDauKhongDen(const string& maDatBan);
    bool xoaDatBan(const string& maDatBan);

    const DatBan* timDatBan(const string& maDatBan) const;
    void timTheoKhachHang(const string& maKH) const;
    void timTheoNgay(const string& ngayDat) const;
    void hienThiDanhSach() const;
    const vector<DatBan>& getDanhSachDatBan() const;
};

#endif
