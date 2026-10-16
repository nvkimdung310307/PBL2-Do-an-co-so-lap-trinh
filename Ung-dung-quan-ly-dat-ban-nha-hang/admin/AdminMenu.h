#pragma once

#include "../services/BanService.h"
#include "../services/DatBanService.h"
#include "../services/KhachHangService.h"
#include "../services/MonAnService.h"
#include "../services/HoaDonService.h"
#include "../taikhoan/TaiKhoanService.h"

using namespace std;

void menuAdmin(
    BanService& banService,
    DatBanService& datBanService,
    KhachHangService& khachHangService,
    MonAnService& monAnService,
    HoaDonService& hoaDonService,
    TaiKhoanService& taiKhoanService
);

