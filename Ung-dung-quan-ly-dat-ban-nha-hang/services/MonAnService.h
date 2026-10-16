
#ifndef MON_AN_SERVICE_H
#define MON_AN_SERVICE_H

#include <vector>
#include <string>
#include "../oop/MonAn.h"

using namespace std;

class MonAnService
{
private:
    vector<MonAn> danhSachMonAn;

public:
    bool themMonAn(const MonAn& mon);
    bool suaMonAn(const string& maMon, const MonAn& monMoi);
    bool xoaMonAn(const string& maMon);

    const MonAn* timMonAn(const string& maMon) const;

    void timTheoTen(const string& tenMon) const;
    void hienThiDanhSach() const;

    bool capNhatTrangThai(
        const string& maMon,
        const string& trangThai
    );

    const vector<MonAn>& getDanhSachMonAn() const;
};

#endif
