#include "KhoService.h"

using namespace std;

bool KhoService::themNguyenLieu(NguyenLieu nl) {
    if (nl.maNL.empty() || nl.tenNL.empty() ||
        nl.soLuong < 0 || nl.mucToiThieu < 0) {
        cout << "Thong tin nguyen lieu khong hop le!\n";
        return false;
    }

    if (timNguyenLieu(nl.maNL) != nullptr) {
        cout << "Ma nguyen lieu da ton tai!\n";
        return false;
    }

    danhSach.push_back(nl);
    cout << "Them nguyen lieu thanh cong!\n";
    return true;
}

NguyenLieu* KhoService::timNguyenLieu(string maNL) {
    for (auto& nl : danhSach) {
        if (nl.maNL == maNL) {
            return &nl;
        }
    }
    return nullptr;
}

bool KhoService::capNhatSoLuong(
    string maNL, int soLuongMoi
) {
    NguyenLieu* nl = timNguyenLieu(maNL);

    if (nl == nullptr || soLuongMoi < 0) {
        cout << "Khong tim thay nguyen lieu hoac so luong sai!\n";
        return false;
    }

    nl->soLuong = soLuongMoi;
    cout << "Cap nhat so luong thanh cong!\n";
    return true;
}

bool KhoService::nhapKho(string maNL, int soLuong) {
    NguyenLieu* nl = timNguyenLieu(maNL);

    if (nl == nullptr || soLuong <= 0) {
        cout << "Thong tin nhap kho khong hop le!\n";
        return false;
    }

    nl->soLuong += soLuong;
    cout << "Nhap kho thanh cong!\n";
    return true;
}

bool KhoService::xuatKho(string maNL, int soLuong) {
    NguyenLieu* nl = timNguyenLieu(maNL);

    if (nl == nullptr || soLuong <= 0) {
        cout << "Thong tin xuat kho khong hop le!\n";
        return false;
    }

    if (nl->soLuong < soLuong) {
        cout << "Khong du nguyen lieu trong kho!\n";
        return false;
    }

    nl->soLuong -= soLuong;
    cout << "Xuat kho thanh cong!\n";
    return true;
}

void KhoService::hienThiKho() {
    if (danhSach.empty()) {
        cout << "Kho chua co nguyen lieu!\n";
        return;
    }

    cout << "\n===== DANH SACH NGUYEN LIEU =====\n";

    for (const auto& nl : danhSach) {
        cout << "Ma: " << nl.maNL
             << " | Ten: " << nl.tenNL
             << " | So luong: " << nl.soLuong
             << " | Don vi: " << nl.donVi
             << " | Toi thieu: " << nl.mucToiThieu
             << '\n';
    }
}

void KhoService::canhBaoHetHang() {
    cout << "\n===== CANH BAO TON KHO =====\n";

    bool coCanhBao = false;

    for (const auto& nl : danhSach) {
        if (nl.soLuong <= nl.mucToiThieu) {
            cout << nl.tenNL
                 << " sap het hang! Con: "
                 << nl.soLuong << ' '
                 << nl.donVi << '\n';

            coCanhBao = true;
        }
    }

    if (!coCanhBao) {
        cout << "Tat ca nguyen lieu deu du!\n";
    }
}
