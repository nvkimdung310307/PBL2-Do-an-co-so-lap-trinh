#include "Ban.h"
#include <vector>

using namespace std;

class QuanLyBan {
private:
    vector<Ban> ds;

public:
    void taoBanMacDinh();

    void hienThiBan();

    void themBan();
    void xoaBan();

    int timViTri(int ma);

    void chonBan();
    void nhanKhach();
    void thanhToan();

    void menuQuanLy();
};
