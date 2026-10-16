
#include "DatBanService.h"
#include <iostream>
#include <sstream>

using namespace std;

// Kiem tra trang thai da huy
namespace
{
    bool daHuy(const string& trangThai)
    {
        return trangThai == "Huy"
            || trangThai == "Hủy"
            || trangThai == "Da huy"
            || trangThai == "Đã hủy";
    }
}

// 1. Kiem tra trung lich
bool DatBanService::trungLich(
    const DatBan& datBan,
    const string& maDatBanBoQua) const
{
    int gioMoi, phutMoi;
    char dauHaiCham;

    string gioDatMoi = datBan.getGioDat();
    stringstream ss(gioDatMoi);
    ss >> gioMoi >> dauHaiCham >> phutMoi;

    if (gioMoi < 0 || gioMoi > 23 ||
        phutMoi < 0 || phutMoi > 59 ||
        dauHaiCham != ':')
        return true;

    int batDauMoi = gioMoi * 60 + phutMoi;
    int ketThucMoi = batDauMoi + 120;

    for (const DatBan& hienTai : danhSachDatBan)
    {
        if (hienTai.getMaDatBan() == maDatBanBoQua)
            continue;

        if (daHuy(hienTai.getTrangThai()))
            continue;

        if (hienTai.getMaBan() != datBan.getMaBan() ||
            hienTai.getNgayDat() != datBan.getNgayDat())
            continue;

        int gioCu, phutCu;
        string gioDatCu = hienTai.getGioDat();

        stringstream ssCu(gioDatCu);
        ssCu >> gioCu >> dauHaiCham >> phutCu;

        int batDauCu = gioCu * 60 + phutCu;
        int ketThucCu = batDauCu + 120;

        if (batDauMoi < ketThucCu &&
            batDauCu < ketThucMoi)
        {
            return true;
        }
    }

    return false;
}

// 2. Them dat ban
bool DatBanService::themDatBan(const DatBan& datBan)
{
    if (!datBan.hopLe())
    {
        cout << "Thong tin dat ban khong hop le!\n";
        return false;
    }

    if (timDatBan(datBan.getMaDatBan()) != nullptr)
    {
        cout << "Ma dat ban da ton tai!\n";
        return false;
    }

    if (trungLich(datBan))
    {
        cout << "Ban da duoc dat vao ngay gio nay!\n";
        return false;
    }

    danhSachDatBan.push_back(datBan);

    cout << "Dat ban thanh cong!\n";
    return true;
}

// 3. Sua thong tin dat ban
bool DatBanService::suaDatBan(
    const string& maDatBan,
    const DatBan& datBanMoi)
{
    for (size_t i = 0; i < danhSachDatBan.size(); i++)
    {
        if (danhSachDatBan[i].getMaDatBan() == maDatBan)
        {
            DatBan capNhat(
                maDatBan,
                datBanMoi.getMaKH(),
                datBanMoi.getMaBan(),
                datBanMoi.getMaNV(),
                datBanMoi.getNgayDat(),
                datBanMoi.getGioDat(),
                datBanMoi.getSoNguoi(),
                datBanMoi.getTrangThai(),
                datBanMoi.getGhiChu()
            );

            if (!capNhat.hopLe())
            {
                cout << "Thong tin moi khong hop le!\n";
                return false;
            }

            if (trungLich(capNhat, maDatBan))
            {
                cout << "Ban da duoc dat vao ngay gio nay!\n";
                return false;
            }

            danhSachDatBan[i] = capNhat;

            cout << "Sua thong tin dat ban thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay lich dat ban!\n";
    return false;
}

// 4. Huy dat ban
bool DatBanService::huyDatBan(const string& maDatBan)
{
    for (size_t i = 0; i < danhSachDatBan.size(); i++)
    {
        if (danhSachDatBan[i].getMaDatBan() == maDatBan)
        {
            if (daHuy(danhSachDatBan[i].getTrangThai()))
            {
                cout << "Lich dat ban da duoc huy truoc do!\n";
                return false;
            }

            danhSachDatBan[i].setTrangThai("Da huy");

            cout << "Huy dat ban thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay lich dat ban!\n";
    return false;
}

// 5. Xoa dat ban
bool DatBanService::xoaDatBan(const string& maDatBan)
{
    for (size_t i = 0; i < danhSachDatBan.size(); i++)
    {
        if (danhSachDatBan[i].getMaDatBan() == maDatBan)
        {
            danhSachDatBan.erase(
                danhSachDatBan.begin() + i
            );

            cout << "Xoa lich dat ban thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay lich dat ban can xoa!\n";
    return false;
}

// 6. Tim theo ma dat ban
const DatBan* DatBanService::timDatBan(
    const string& maDatBan) const
{
    for (size_t i = 0; i < danhSachDatBan.size(); i++)
    {
        if (danhSachDatBan[i].getMaDatBan() == maDatBan)
        {
            return &danhSachDatBan[i];
        }
    }

    return nullptr;
}

// 7. Tim theo ma khach hang
void DatBanService::timTheoKhachHang(
    const string& maKH) const
{
    bool timThay = false;

    for (size_t i = 0; i < danhSachDatBan.size(); i++)
    {
        if (danhSachDatBan[i].getMaKH() == maKH)
        {
            const DatBan& db = danhSachDatBan[i];

            cout << "Ma dat ban: " << db.getMaDatBan() << '\n';
            cout << "Ma ban: " << db.getMaBan() << '\n';
            cout << "Ngay dat: " << db.getNgayDat() << '\n';
            cout << "Gio dat: " << db.getGioDat() << '\n';
            cout << "Trang thai: " << db.getTrangThai() << '\n';
            cout << "-----------------------------\n";

            timThay = true;
        }
    }

    if (!timThay)
        cout << "Khong tim thay lich dat cua khach hang!\n";
}

// 8. Tim theo ngay dat
void DatBanService::timTheoNgay(
    const string& ngayDat) const
{
    bool timThay = false;

    for (size_t i = 0; i < danhSachDatBan.size(); i++)
    {
        if (danhSachDatBan[i].getNgayDat() == ngayDat)
        {
            const DatBan& db = danhSachDatBan[i];

            cout << "Ma dat ban: " << db.getMaDatBan() << '\n';
            cout << "Ma khach hang: " << db.getMaKH() << '\n';
            cout << "Ma ban: " << db.getMaBan() << '\n';
            cout << "Gio dat: " << db.getGioDat() << '\n';
            cout << "So nguoi: " << db.getSoNguoi() << '\n';
            cout << "Trang thai: " << db.getTrangThai() << '\n';
            cout << "-----------------------------\n";

            timThay = true;
        }
    }

    if (!timThay)
        cout << "Khong co lich dat vao ngay nay!\n";
}

// 9. Hien thi danh sach dat ban
void DatBanService::hienThiDanhSach() const
{
    if (danhSachDatBan.empty())
    {
        cout << "Danh sach dat ban dang rong!\n";
        return;
    }

    cout << "\n========== DANH SACH DAT BAN ==========\n";

    for (size_t i = 0; i < danhSachDatBan.size(); i++)
    {
        const DatBan& db = danhSachDatBan[i];

        cout << "Ma dat ban: " << db.getMaDatBan() << '\n';
        cout << "Ma khach hang: " << db.getMaKH() << '\n';
        cout << "Ma ban: " << db.getMaBan() << '\n';
        cout << "Ma nhan vien: " << db.getMaNV() << '\n';
        cout << "Ngay dat: " << db.getNgayDat() << '\n';
        cout << "Gio dat: " << db.getGioDat() << '\n';
        cout << "So nguoi: " << db.getSoNguoi() << '\n';
        cout << "Trang thai: " << db.getTrangThai() << '\n';
        cout << "Ghi chu: " << db.getGhiChu() << '\n';
        cout << "---------------------------------------\n";
    }
}

// 10. Lay danh sach dat ban
const vector<DatBan>&
DatBanService::getDanhSachDatBan() const
{
    return danhSachDatBan;
}
