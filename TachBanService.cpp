
#include "TachBanService.h"
#include "../oop/Ban.h"
#include <iostream>
using namespace std;

TachBanService::TachBanService(BanService& ban)
    : banService(ban)
{
}

bool TachBanService::tachBan(
    const string& maBanGoc,
    const string& maBanMoi)
{
    if (maBanGoc == maBanMoi)
    {
        cout << "Ban goc va ban moi khong duoc trung nhau!\n";
        return false;
    }

    const vector<Ban>& danhSach = banService.getDanhSachBan();

    const Ban* banGoc = nullptr;
    const Ban* banMoi = nullptr;

    for (const Ban& ban : danhSach)
    {
        if (ban.getMaBan() == maBanGoc)
            banGoc = &ban;

        if (ban.getMaBan() == maBanMoi)
            banMoi = &ban;
    }

    if (banGoc == nullptr || banMoi == nullptr)
    {
        cout << "Khong tim thay ban goc hoac ban moi!\n";
        return false;
    }

    if (banMoi->getTrangThai() != "Trong" &&
        banMoi->getTrangThai() != "Trống")
    {
        cout << "Ban moi phai dang trong!\n";
        return false;
    }

    cout << "Da kiem tra ban goc: " << maBanGoc << '\n';
    cout << "Da kiem tra ban moi: " << maBanMoi << '\n';

    cout << "Chua the tach ban hoan tat neu chua xu ly "
         << "khach va hoa don lien quan.\n";

    return false;
}

