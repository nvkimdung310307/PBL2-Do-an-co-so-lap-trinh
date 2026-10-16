
#include "MonAnService.h"
#include <iostream>

using namespace std;

// 1. Them mon an
bool MonAnService::themMonAn(const MonAn& mon)
{
    if (!mon.hopLe())
    {
        cout << "Thong tin mon an khong hop le!\n";
        return false;
    }

    if (timMonAn(mon.getMaMon()) != nullptr)
    {
        cout << "Ma mon an da ton tai!\n";
        return false;
    }

    danhSachMonAn.push_back(mon);

    cout << "Them mon an thanh cong!\n";
    return true;
}

// 2. Sua mon an
bool MonAnService::suaMonAn(
    const string& maMon,
    const MonAn& monMoi)
{
    for (size_t i = 0; i < danhSachMonAn.size(); i++)
    {
        if (danhSachMonAn[i].getMaMon() == maMon)
        {
            MonAn monCapNhat(
                maMon,
                monMoi.getTenMon(),
                monMoi.getLoaiMon(),
                monMoi.getDonGia(),
                monMoi.getDonViTinh(),
                monMoi.getMoTa(),
                monMoi.getHinhAnh(),
                monMoi.getTrangThai()
            );

            if (!monCapNhat.hopLe())
            {
                cout << "Thong tin mon an moi khong hop le!\n";
                return false;
            }

            danhSachMonAn[i] = monCapNhat;

            cout << "Sua mon an thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay mon an can sua!\n";
    return false;
}

// 3. Xoa mon an
bool MonAnService::xoaMonAn(const string& maMon)
{
    for (size_t i = 0; i < danhSachMonAn.size(); i++)
    {
        if (danhSachMonAn[i].getMaMon() == maMon)
        {
            danhSachMonAn.erase(
                danhSachMonAn.begin() + i
            );

            cout << "Xoa mon an thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay mon an can xoa!\n";
    return false;
}

// 4. Tim mon an theo ma
const MonAn* MonAnService::timMonAn(
    const string& maMon) const
{
    for (size_t i = 0; i < danhSachMonAn.size(); i++)
    {
        if (danhSachMonAn[i].getMaMon() == maMon)
        {
            return &danhSachMonAn[i];
        }
    }

    return nullptr;
}

// 5. Tim mon theo ten
void MonAnService::timTheoTen(const string& tenMon) const
{
    bool timThay = false;

    for (size_t i = 0; i < danhSachMonAn.size(); i++)
    {
        if (danhSachMonAn[i].getTenMon().find(tenMon)
            != string::npos)
        {
            cout << "Ma mon: "
                 << danhSachMonAn[i].getMaMon() << '\n';

            cout << "Ten mon: "
                 << danhSachMonAn[i].getTenMon() << '\n';

            cout << "Loai mon: "
                 << danhSachMonAn[i].getLoaiMon() << '\n';

            cout << "Don gia: "
                 << danhSachMonAn[i].getDonGia() << '\n';

            cout << "Trang thai: "
                 << danhSachMonAn[i].getTrangThai() << '\n';

            cout << "-----------------------------\n";
            timThay = true;
        }
    }

    if (!timThay)
        cout << "Khong tim thay mon an!\n";
}

// 6. Hien thi danh sach mon an
void MonAnService::hienThiDanhSach() const
{
    if (danhSachMonAn.empty())
    {
        cout << "Danh sach mon an dang rong!\n";
        return;
    }

    cout << "\n========== DANH SACH MON AN ==========\n";

    for (size_t i = 0; i < danhSachMonAn.size(); i++)
    {
        const MonAn& mon = danhSachMonAn[i];

        cout << "Ma mon: " << mon.getMaMon() << '\n';
        cout << "Ten mon: " << mon.getTenMon() << '\n';
        cout << "Loai mon: " << mon.getLoaiMon() << '\n';
        cout << "Don gia: " << mon.getDonGia() << '\n';
        cout << "Don vi tinh: " << mon.getDonViTinh() << '\n';
        cout << "Mo ta: " << mon.getMoTa() << '\n';
        cout << "Hinh anh: " << mon.getHinhAnh() << '\n';
        cout << "Trang thai: " << mon.getTrangThai() << '\n';
        cout << "--------------------------------------\n";
    }
}

// 7. Cap nhat trang thai mon an
bool MonAnService::capNhatTrangThai(
    const string& maMon,
    const string& trangThai)
{
    for (size_t i = 0; i < danhSachMonAn.size(); i++)
    {
        if (danhSachMonAn[i].getMaMon() == maMon)
        {
            MonAn monCapNhat(
                danhSachMonAn[i].getMaMon(),
                danhSachMonAn[i].getTenMon(),
                danhSachMonAn[i].getLoaiMon(),
                danhSachMonAn[i].getDonGia(),
                danhSachMonAn[i].getDonViTinh(),
                danhSachMonAn[i].getMoTa(),
                danhSachMonAn[i].getHinhAnh(),
                trangThai
            );

            if (!monCapNhat.hopLe())
            {
                cout << "Thong tin trang thai khong hop le!\n";
                return false;
            }

            danhSachMonAn[i].setTrangThai(trangThai);

            cout << "Cap nhat trang thai mon an thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay mon an!\n";
    return false;
}

// 8. Lay danh sach mon an
const vector<MonAn>&
MonAnService::getDanhSachMonAn() const
{
    return danhSachMonAn;
}
