
#include "BaoCaoService.h"

using namespace std;

BaoCaoService::BaoCaoService(BanService& banService)
    : banService(banService) {
}

void BaoCaoService::thongKeBan() {
    const auto& danhSach = banService.getDanhSachBan();

    int tongBan = 0;
    int banTrong = 0;
    int banDangSuDung = 0;
    int banDaDat = 0;
    int banBaoTri = 0;

    for (const auto& ban : danhSach) {
        tongBan++;

        string trangThai = ban.getTrangThai();

        if (trangThai == "Trong" || trangThai == "Trống") {
            banTrong++;
        }
        else if (trangThai == "Dang su dung" ||
                 trangThai == "Đang sử dụng") {
            banDangSuDung++;
        }
        else if (trangThai == "Da dat" ||
                 trangThai == "Đã đặt") {
            banDaDat++;
        }
        else if (trangThai == "Bao tri" ||
                 trangThai == "Bảo trì") {
            banBaoTri++;
        }
    }

    cout << "\n========== BAO CAO BAN ==========\n";
    cout << "Tong so ban: " << tongBan << '\n';
    cout << "Ban trong: " << banTrong << '\n';
    cout << "Ban dang su dung: " << banDangSuDung << '\n';
    cout << "Ban da dat: " << banDaDat << '\n';
    cout << "Ban bao tri: " << banBaoTri << '\n';
    cout << "=================================\n";
}
