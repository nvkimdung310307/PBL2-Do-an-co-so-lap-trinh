
#pragma once

#include "../services/BanService.h"
#include "../services/DatBanService.h"
#include "../services/KhachHangService.h"
#include "../services/MonAnService.h"
#include "../services/HoaDonService.h"

using namespace std;

void menuNhanVien(
    BanService& banService,
    DatBanService& datBanService,
    KhachHangService& khachHangService,
    MonAnService& monAnService,
    HoaDonService& hoaDonService
);
