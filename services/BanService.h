
#ifndef BAN_SERVICE_H
#define BAN_SERVICE_H

#include <vector>
#include <string>
#include "../oop/Ban.h"

using namespace std;

class BanService
{
private:
    mutable vector<Ban> danhSachBan;

public:
    bool themBan(const Ban& ban);
    bool suaBan(const string& maBan, const Ban& banMoi);
    bool xoaBan(const string& maBan);

    const Ban* timBan(const string& maBan) const;

    void hienThiDanhSach() const;
    void timBanTrong(int soChoToiThieu) const;

    void timBanTheoKhuVuc(
        int soChoToiThieu,
        const string& maKhuVuc
    ) const;

    bool capNhatTrangThai(
        const string& maBan,
        const string& trangThai
    );

    const vector<Ban>& getDanhSachBan() const;
};

#endif
