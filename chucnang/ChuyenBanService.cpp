#include "ChuyenBanService.h"

using namespace std;

ChuyenBanService::ChuyenBanService(BanService& banService, DatBanService& datBanService)
    : banService(banService), datBanService(datBanService)
{
}

bool ChuyenBanService::chuyenBan(const string& maDatBan, const string& maBanMoi)
{
    if (maDatBan.empty() || maBanMoi.empty())
    {
        cout << "Thong tin khong duoc de trong!\n";
        return false;
    }

    const DatBan* datBan = datBanService.timDatBan(maDatBan);
    if (datBan == nullptr)
    {
        cout << "Khong tim thay lich dat ban!\n";
        return false;
    }

    if (datBan->getTrangThai() != "Đã đặt" && datBan->getTrangThai() != "Da dat")
    {
        cout << "Chi duoc chuyen lich dang o trang thai Da dat.\n";
        return false;
    }

    const string maBanCu = datBan->getMaBan();
    if (maBanCu == maBanMoi)
    {
        cout << "Ban moi phai khac ban hien tai!\n";
        return false;
    }

    // Kiem tra va chuyen ban trong mot transaction SQL; khong dua vao trang thai cache.
    if (!datBanService.chuyenBanSQL(maDatBan, maBanMoi))
    {
        cout << "Khong the chuyen ban; SQL Server tu choi cap nhat.\n";
        return false;
    }

    cout << "Chuyen ban thanh cong. Ban cu: " << maBanCu
         << " -> ban moi: " << maBanMoi << '\n';
    return true;
}
