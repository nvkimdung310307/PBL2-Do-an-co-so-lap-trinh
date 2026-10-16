
#ifndef BAN_SERVICE_H
#define BAN_SERVICE_H

#include <vector>
#include <string>
#include "../oop/Ban.h"

using namespace std;

class BanService
{
private:
    vector<Ban> danhSachBan;

public:
    // Them ban
    bool themBan(const Ban& ban);

    // Sua thong tin ban
    bool suaBan(const string& maBan, const Ban& banMoi);

    // Xoa ban
    bool xoaBan(const string& maBan);

    // Tim ban theo ma
    const Ban* timBan(const string& maBan) const;

    // Hien thi danh sach
    void hienThiDanhSach() const;

    // Tim ban trong theo so cho
    void timBanTrong(int soChoToiThieu) const;

    // Cap nhat trang thai ban
    bool capNhatTrangThai(
        const string& maBan,
        const string& trangThai
    );

    // Lay danh sach ban
    const vector<Ban>& getDanhSachBan() const;
};

#endif
