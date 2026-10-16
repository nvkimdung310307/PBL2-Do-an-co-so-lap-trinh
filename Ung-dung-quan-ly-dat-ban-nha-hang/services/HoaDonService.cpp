
#include "HoaDonService.h"
#include <iostream>
#include <iomanip>

using namespace std;

// 1. Tim hoa don
const HoaDon* HoaDonService::timHoaDon(
    const string& maHD) const
{
    for (size_t i = 0; i < danhSachHoaDon.size(); i++)
    {
        if (danhSachHoaDon[i].getMaHD() == maHD)
            return &danhSachHoaDon[i];
    }

    return nullptr;
}

// 2. Tao hoa don
bool HoaDonService::taoHoaDon(const HoaDon& hd)
{
    if (!hd.hopLe())
    {
        cout << "Thong tin hoa don khong hop le!\n";
        return false;
    }

    if (timHoaDon(hd.getMaHD()) != nullptr)
    {
        cout << "Ma hoa don da ton tai!\n";
        return false;
    }

    // Hoa don moi chua thanh toan
    HoaDon hoaDonMoi = hd;
    hoaDonMoi.setTrangThai("Chua thanh toan");
    hoaDonMoi.setTongTien(0);
    hoaDonMoi.setTienGiam(0);
    hoaDonMoi.setThanhTien(0);

    danhSachHoaDon.push_back(hoaDonMoi);

    cout << "Tao hoa don thanh cong!\n";
    return true;
}

// 3. Them mon vao hoa don
bool HoaDonService::themChiTiet(const ChiTietHoaDon& ct)
{
    if (!ct.hopLe())
    {
        cout << "Chi tiet hoa don khong hop le!\n";
        return false;
    }

    const HoaDon* hd = timHoaDon(ct.getMaHD());

    if (hd == nullptr)
    {
        cout << "Hoa don khong ton tai!\n";
        return false;
    }

    if (hd->getTrangThai() == "Da thanh toan"
        || hd->getTrangThai() == "Đã thanh toán")
    {
        cout << "Hoa don da thanh toan, khong the them mon!\n";
        return false;
    }

    // Khong them trung mot mon trong cung hoa don
    for (size_t i = 0; i < danhSachChiTiet.size(); i++)
    {
        if (danhSachChiTiet[i].getMaHD() == ct.getMaHD()
            && danhSachChiTiet[i].getMaMon() == ct.getMaMon())
        {
            cout << "Mon da co trong hoa don!\n";
            cout << "Hay sua so luong neu muon goi them.\n";
            return false;
        }
    }

    danhSachChiTiet.push_back(ct);

    cout << "Them mon vao hoa don thanh cong!\n";
    return true;
}

// 4. Xoa mon khoi hoa don
bool HoaDonService::xoaChiTiet(
    const string& maHD,
    const string& maMon)
{
    const HoaDon* hd = timHoaDon(maHD);

    if (hd == nullptr)
    {
        cout << "Hoa don khong ton tai!\n";
        return false;
    }

    if (hd->getTrangThai() == "Da thanh toan"
        || hd->getTrangThai() == "Đã thanh toán")
    {
        cout << "Khong the sua hoa don da thanh toan!\n";
        return false;
    }

    for (size_t i = 0; i < danhSachChiTiet.size(); i++)
    {
        if (danhSachChiTiet[i].getMaHD() == maHD
            && danhSachChiTiet[i].getMaMon() == maMon)
        {
            danhSachChiTiet.erase(
                danhSachChiTiet.begin() + i
            );

            cout << "Xoa mon khoi hoa don thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay mon trong hoa don!\n";
    return false;
}

// 5. Tinh tien
bool HoaDonService::tinhTien(
    const string& maHD,
    double tienGiam)
{
    if (tienGiam < 0)
    {
        cout << "Tien giam khong hop le!\n";
        return false;
    }

    for (size_t i = 0; i < danhSachHoaDon.size(); i++)
    {
        HoaDon& hd = danhSachHoaDon[i];

        if (hd.getMaHD() == maHD)
        {
            if (hd.getTrangThai() == "Da thanh toan"
                || hd.getTrangThai() == "Đã thanh toán")
            {
                cout << "Hoa don da thanh toan!\n";
                return false;
            }

            double tongTien = 0;

            for (size_t j = 0; j < danhSachChiTiet.size(); j++)
            {
                if (danhSachChiTiet[j].getMaHD() == maHD)
                {
                    tongTien +=
                        danhSachChiTiet[j].getSoLuong()
                        * danhSachChiTiet[j].getDonGia();
                }
            }

            if (tienGiam > tongTien)
            {
                cout << "Tien giam khong duoc lon hon tong tien!\n";
                return false;
            }

            hd.setTongTien(tongTien);
            hd.setTienGiam(tienGiam);
            hd.setThanhTien(tongTien - tienGiam);

            cout << fixed << setprecision(0);
            cout << "Tong tien: " << tongTien << '\n';
            cout << "Tien giam: " << tienGiam << '\n';
            cout << "Thanh tien: "
                 << hd.getThanhTien() << '\n';

            return true;
        }
    }

    cout << "Khong tim thay hoa don!\n";
    return false;
}

// 6. Thanh toan
bool HoaDonService::thanhToan(
    const string& maHD,
    const string& phuongThuc)
{
    for (size_t i = 0; i < danhSachHoaDon.size(); i++)
    {
        HoaDon& hd = danhSachHoaDon[i];

        if (hd.getMaHD() == maHD)
        {
            if (hd.getTrangThai() == "Da thanh toan"
                || hd.getTrangThai() == "Đã thanh toán")
            {
                cout << "Hoa don da thanh toan truoc do!\n";
                return false;
            }

            if (phuongThuc.empty())
            {
                cout << "Vui long nhap phuong thuc thanh toan!\n";
                return false;
            }

            bool coChiTiet = false;

            for (size_t j = 0; j < danhSachChiTiet.size(); j++)
            {
                if (danhSachChiTiet[j].getMaHD() == maHD)
                {
                    coChiTiet = true;
                    break;
                }
            }

            if (!coChiTiet)
            {
                cout << "Hoa don chua co mon an!\n";
                return false;
            }

            hd.setPhuongThucThanhToan(phuongThuc);
            hd.setTrangThai("Da thanh toan");

            cout << "Thanh toan thanh cong!\n";
            cout << "So tien phai tra: "
                 << hd.getThanhTien() << '\n';

            return true;
        }
    }

    cout << "Khong tim thay hoa don!\n";
    return false;
}

// 7. Hien thi danh sach hoa don
void HoaDonService::hienThiDanhSach() const
{
    if (danhSachHoaDon.empty())
    {
        cout << "Danh sach hoa don dang rong!\n";
        return;
    }

    for (size_t i = 0; i < danhSachHoaDon.size(); i++)
    {
        const HoaDon& hd = danhSachHoaDon[i];

        cout << "\nMa hoa don: " << hd.getMaHD() << '\n';
        cout << "Ma dat ban: " << hd.getMaDatBan() << '\n';
        cout << "Ngay lap: " << hd.getNgayLap() << '\n';
        cout << "Tong tien: " << hd.getTongTien() << '\n';
        cout << "Tien giam: " << hd.getTienGiam() << '\n';
        cout << "Thanh tien: " << hd.getThanhTien() << '\n';
        cout << "Phuong thuc: "
             << hd.getPhuongThucThanhToan() << '\n';
        cout << "Trang thai: " << hd.getTrangThai() << '\n';
        cout << "--------------------------------\n";
    }
}

// 8. Hien thi chi tiet hoa don
void HoaDonService::hienThiChiTiet(
    const string& maHD) const
{
    if (timHoaDon(maHD) == nullptr)
    {
        cout << "Hoa don khong ton tai!\n";
        return;
    }

    bool coChiTiet = false;

    cout << "\n===== CHI TIET HOA DON "
         << maHD << " =====\n";

    for (size_t i = 0; i < danhSachChiTiet.size(); i++)
    {
        const ChiTietHoaDon& ct = danhSachChiTiet[i];

        if (ct.getMaHD() == maHD)
        {
            cout << "Ma mon: " << ct.getMaMon() << '\n';
            cout << "So luong: " << ct.getSoLuong() << '\n';
            cout << "Don gia: " << ct.getDonGia() << '\n';
            cout << "Thanh tien mon: "
                 << ct.getSoLuong() * ct.getDonGia() << '\n';
            cout << "-----------------------------\n";

            coChiTiet = true;
        }
    }

    if (!coChiTiet)
        cout << "Hoa don chua co mon an!\n";
}

// 9. Lay danh sach hoa don
const vector<HoaDon>&
HoaDonService::getDanhSachHoaDon() const
{
    return danhSachHoaDon;
}

// 10. Lay danh sach chi tiet
const vector<ChiTietHoaDon>&
HoaDonService::getDanhSachChiTiet() const
{
    return danhSachChiTiet;
}
