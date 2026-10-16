#pragma once
#include <iostream>
#include <vector>
#include "../oop/KhachCho.h"
using namespace std;

class KhachChoService
{
private:
    vector<KhachCho> danhSachKhachCho;

public:
    void themKhachCho(const KhachCho& khach);
    void hienThiDanhSach() const;
    bool goiKhach(const string& maKhachCho);
    bool huyKhachCho(const string& maKhachCho);
};

