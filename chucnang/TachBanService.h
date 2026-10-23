#ifndef TACHBANSERVICE_H
#define TACHBANSERVICE_H

#include <iostream>
#include <string>
using namespace std;

#include "../services/BanService.h"

class TachBanService
{
private:
    BanService& banService;

public:
    TachBanService(BanService& ban);

    bool tachBan(
        const string& maBanGoc,
        const string& maBanMoi
    );
};

#endif
