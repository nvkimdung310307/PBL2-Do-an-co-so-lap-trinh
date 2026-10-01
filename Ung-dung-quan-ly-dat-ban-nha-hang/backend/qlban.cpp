#include "QuanLyBan.h"
#include <iostream>
#include <windows.h>

using namespace std;

void QuanLyBan::taoBanMacDinh() {

    // Tao san 10 ban
    for (int i = 1; i <= 10; i++) {

        Ban b(i, 4);

        ds.push_back(b);
    }
}
