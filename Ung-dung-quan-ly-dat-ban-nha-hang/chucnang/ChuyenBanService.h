#pragma once
#include <iostream>
#include <string>
#include "../services/BanService.h"

using namespace std;

class ChuyenBanService {
private:
    BanService& banService;

public:
    ChuyenBanService(BanService& banService);

    bool chuyenBan(string maBanCu, string maBanMoi);
};
