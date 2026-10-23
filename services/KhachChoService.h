#pragma once
#include <string>
#include "../oop/KhachCho.h"

class KhachChoService
{
public:
    bool themKhachCho(const KhachCho& khach);
    void hienThiDanhSach() const;
    bool goiKhach(const std::string& maKhachCho);
    bool huyKhachCho(const std::string& maKhachCho);
};
