#ifndef GOPBANSERVICE_H
#define GOPBANSERVICE_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "../services/BanService.h"

class GopBanService
{
private:
    BanService& banService;

public:
    GopBanService(BanService& ban);

    bool gopBan(
        const vector<string>& danhSachMaBan,
        const string& maBanDich
    );
};

#endif
