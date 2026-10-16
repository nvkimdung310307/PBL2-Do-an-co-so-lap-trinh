
#include "ChuyenBanService.h"

using namespace std;

ChuyenBanService::ChuyenBanService(
    BanService& banService,
    DatBanService& datBanService
)
    : banService(banService),
      datBanService(datBanService) {
}

bool ChuyenBanService::chuyenBan(
    const string& maDatBan,
    const string& maBanMoi
) {
    if (maDatBan.empty() || maBanMoi.empty()) {
        cout << "Thong tin khong duoc de trong!\n";
        return false;
    }

    const DatBan* datBan = datBanService.timDatBan(maDatBan);

    if (datBan == nullptr) {
        cout << "Khong tim thay lich dat ban!\n";
        return false;
    }

    string trangThaiDat = datBan->getTrangThai();

    if (trangThaiDat == "Da huy" ||
        trangThaiDat == "Đã hủy" ||
        trangThaiDat == "Huy" ||
        trangThaiDat == "Hủy" ||
        trangThaiDat == "KhongDen" ||
        trangThaiDat == "Khong den" ||
        trangThaiDat == "Không đến") {
        cout << "Lich dat khong con hieu luc!\n";
        return false;
    }

    string maBanCu = datBan->getMaBan();

    if (maBanCu == maBanMoi) {
        cout << "Ban moi phai khac ban hien tai!\n";
        return false;
    }

    const Ban* banCu = banService.timBan(maBanCu);
    const Ban* banMoi = banService.timBan(maBanMoi);

    if (banCu == nullptr || banMoi == nullptr) {
        cout << "Khong tim thay ban!\n";
        return false;
    }

    if (banMoi->getTrangThai() != "Trong" &&
        banMoi->getTrangThai() != "Trống") {
        cout << "Ban moi khong trong!\n";
        return false;
    }

    cout << "Da kiem tra du dieu kien chuyen ban.\n";
    cout << "Ma dat ban: " << maDatBan << '\n';
    cout << "Ban cu: " << maBanCu << '\n';
    cout << "Ban moi: " << maBanMoi << '\n';

    cout << "Chuc nang can cap nhat lich dat va trang thai ban.\n";

    return false;
}
