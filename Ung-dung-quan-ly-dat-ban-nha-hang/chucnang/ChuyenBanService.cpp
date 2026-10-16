
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

// Tao thong tin dat ban moi, chi thay ma ban
DatBan datBanMoi(
    datBan->getMaDatBan(),
    datBan->getMaKH(),
    maBanMoi,
    datBan->getMaNV(),
    datBan->getNgayDat(),
    datBan->getGioDat(),
    datBan->getSoNguoi(),
    datBan->getTrangThai(),
    datBan->getGhiChu()
);

// Cap nhat lich dat ban truoc
if (!datBanService.suaDatBan(maDatBan, datBanMoi))
{
    cout << "Khong the cap nhat lich dat ban!\n";
    return false;
}

// Cap nhat trang thai ban cu
if (!banService.capNhatTrangThai(maBanCu, "Trong"))
{
    // Khoi phuc lich dat ve ban cu neu cap nhat trang thai that bai
    datBanService.suaDatBan(maDatBan, *datBan);
    cout << "Khong the cap nhat trang thai ban cu!\n";
    return false;
}

// Cap nhat trang thai ban moi
if (!banService.capNhatTrangThai(maBanMoi, "Dang su dung"))
{
    // Khoi phuc du lieu neu cap nhat ban moi that bai
    banService.capNhatTrangThai(maBanCu, "Dang su dung");
    datBanService.suaDatBan(maDatBan, *datBan);
    cout << "Khong the cap nhat trang thai ban moi!\n";
    return false;
}

cout << "Chuyen ban thanh cong!\n";
cout << "Ban cu: " << maBanCu << " -> Trong\n";
cout << "Ban moi: " << maBanMoi << " -> Dang su dung\n";

return true;
}
