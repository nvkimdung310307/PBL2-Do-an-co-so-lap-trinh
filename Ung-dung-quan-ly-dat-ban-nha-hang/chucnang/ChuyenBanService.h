
#pragma once
#include <iostream>
#include <string>
#include "../services/BanService.h"
#include "../services/DatBanService.h"

using namespace std;

class ChuyenBanService {
private:
    BanService& banService;
    DatBanService& datBanService;

public:
    ChuyenBanService(
        BanService& banService,
        DatBanService& datBanService
    );

    bool chuyenBan(
        const string& maDatBan,
        const string& maBanMoi
    );
};
