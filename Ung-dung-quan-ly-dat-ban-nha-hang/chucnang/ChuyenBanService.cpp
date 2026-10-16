#include "ChuyenBanService.h"

using namespace std;

ChuyenBanService::ChuyenBanService(BanService& banService)
    : banService(banService) {
}

bool ChuyenBanService::chuyenBan(string maBanCu, string maBanMoi) {
    if (maBanCu.empty() || maBanMoi.empty()) {
        cout << "Ma ban khong duoc de trong!\n";
        return false;
    }

    if (maBanCu == maBanMoi) {
        cout << "Ban moi phai khac ban cu!\n";
        return false;
    }

    const Ban* banCu = banService.timBan(maBanCu);
    const Ban* banMoi = banService.timBan(maBanMoi);

    if (banCu == nullptr || banMoi == nullptr) {
        cout << "Khong tim thay ban!\n";
        return false;
    }

    string trangThaiCu = banCu->getTrangThai();
    string trangThaiMoi = banMoi->getTrangThai();

    if (trangThaiCu != "Dang su dung" &&
        trangThaiCu != "Đang sử dụng") {
        cout << "Ban cu khong co khach dang su dung!\n";
        return false;
    }

    if (trangThaiMoi != "Trong" &&
        trangThaiMoi != "Trống") {
        cout << "Ban moi khong trong!\n";
        return false;
    }

    cout << "Da kiem tra dieu kien chuyen ban.\n";
    cout << "Can cap nhat ca dat ban va hoa don truoc khi hoan tat.\n";

    return false;
}
