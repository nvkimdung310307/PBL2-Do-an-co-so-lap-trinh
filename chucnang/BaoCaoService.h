#pragma once
#include <iostream>
#include "../services/BanService.h"

using namespace std;

class BaoCaoService {
private:
    BanService& banService;

public:
    BaoCaoService(BanService& banService);

    void thongKeBan();
};
