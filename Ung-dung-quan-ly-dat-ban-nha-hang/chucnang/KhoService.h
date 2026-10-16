#pragma once
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct NguyenLieu {
    string maNL;
    string tenNL;
    string donVi;
    int soLuong;
    int mucToiThieu;
};

class KhoService {
private:
    vector<NguyenLieu> danhSach;

public:
    bool themNguyenLieu(NguyenLieu nl);
    bool capNhatSoLuong(string maNL, int soLuongMoi);
    bool nhapKho(string maNL, int soLuong);
    bool xuatKho(string maNL, int soLuong);

    NguyenLieu* timNguyenLieu(string maNL);
    void hienThiKho();
    void canhBaoHetHang();
};
