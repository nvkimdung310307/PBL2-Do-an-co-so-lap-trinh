#include "GopBanService.h"
#include "../oop/Ban.h"
#include <iostream>
using namespace std;

GopBanService::GopBanService(BanService& ban)
    : banService(ban)
{
}

bool GopBanService::gopBan(
    const vector<string>& danhSachMaBan,
    const string& maBanDich)
{
    if (danhSachMaBan.size() < 2)
    {
        cout << "Phai chon it nhat 2 ban de gop!\n";
        return false;
    }

    const vector<Ban>& danhSach = banService.getDanhSachBan();

    const Ban* banDich = nullptr;

    for (const Ban& ban : danhSach)
    {
        if (ban.getMaBan() == maBanDich)
        {
            banDich = &ban;
            break;
        }
    }

    if (banDich == nullptr)
    {
        cout << "Khong tim thay ban dich!\n";
        return false;
    }

    // Kiem tra ban dich co nam trong danh sach gop khong
    bool coBanDich = false;

    for (const string& maBan : danhSachMaBan)
    {
        if (maBan == maBanDich)
        {
            coBanDich = true;
            break;
        }
    }

    if (!coBanDich)
    {
        cout << "Ban dich phai nam trong danh sach ban gop!\n";
        return false;
    }

    // Kiem tra cac ban co ton tai va khong trung ma
    for (size_t i = 0; i < danhSachMaBan.size(); ++i)
    {
        for (size_t j = i + 1; j < danhSachMaBan.size(); ++j)
        {
            if (danhSachMaBan[i] == danhSachMaBan[j])
            {
                cout << "Danh sach co ma ban bi trung!\n";
                return false;
            }
        }

        bool timThay = false;

        for (const Ban& ban : danhSach)
        {
            if (ban.getMaBan() == danhSachMaBan[i])
            {
                timThay = true;
                break;
            }
        }

        if (!timThay)
        {
            cout << "Khong tim thay ban: "
                 << danhSachMaBan[i] << '\n';
            return false;
        }
    }

// Kiem tra tat ca ban can gop dang duoc su dung
for (const string& maBan : danhSachMaBan)
{
    const Ban* ban = banService.timBan(maBan);

    if (ban == nullptr)
    {
        cout << "Khong tim thay ban: " << maBan << '\n';
        return false;
    }

    string trangThai = ban->getTrangThai();

    if (trangThai != "Dang su dung" &&
        trangThai != "Đang sử dụng")
    {
        cout << "Ban " << maBan
             << " chua duoc su dung, khong the gop!\n";
        return false;
    }
}

// Giu ban dich, cac ban con lai chuyen ve trong
for (const string& maBan : danhSachMaBan)
{
    if (maBan != maBanDich)
    {
        if (!banService.capNhatTrangThai(maBan, "Trong"))
        {
            cout << "Gop ban that bai khi cap nhat "
                 << maBan << "!\n";
            return false;
        }
    }
}

cout << "Gop ban thanh cong!\n";
cout << "Ban dich duoc giu lai: " << maBanDich << '\n';

return true;
}

