#include "ThongKeDatBanService.h"

void ThongKeDatBanService::thongKeTongQuan(
    const DatBanService& quanLyDatBan) const
{
    const vector<DatBan>& danhSach =
        quanLyDatBan.getDanhSachDatBan();

    int tong = danhSach.size();
    int daHuy = 0;
    int khongDen = 0;
    int daDen = 0;
    int dangCho = 0;

    for (size_t i = 0; i < danhSach.size(); i++)
    {
        string trangThai = danhSach[i].getTrangThai();

        if (trangThai == "Da huy" ||
            trangThai == "Đã hủy" ||
            trangThai == "Huy" ||
            trangThai == "Hủy")
        {
            daHuy++;
        }
        else if (trangThai == "KhongDen" ||
                 trangThai == "Khong den" ||
                 trangThai == "Không đến")
        {
            khongDen++;
        }
        else if (trangThai == "DaDen" ||
                 trangThai == "Da den" ||
                 trangThai == "Đã đến")
        {
            daDen++;
        }
        else
        {
            dangCho++;
        }
    }

    cout << "\n========== THONG KE DAT BAN ==========\n";
    cout << "Tong so luot dat: " << tong << '\n';
    cout << "So luot da huy: " << daHuy << '\n';
    cout << "So luot khach khong den: " << khongDen << '\n';
    cout << "So luot khach da den: " << daDen << '\n';
    cout << "So luot con lai: " << dangCho << '\n';

    if (tong > 0)
    {
        double tyLeHuy = daHuy * 100.0 / tong;
        double tyLeKhongDen = khongDen * 100.0 / tong;

        cout << "Ty le huy: " << tyLeHuy << "%\n";
        cout << "Ty le khong den: "
             << tyLeKhongDen << "%\n";
    }
}

void ThongKeDatBanService::thongKeTheoNgay(
    const DatBanService& quanLyDatBan,
    const string& ngay) const
{
    const vector<DatBan>& danhSach =
        quanLyDatBan.getDanhSachDatBan();

    int tong = 0;
    int daHuy = 0;
    int khongDen = 0;
    int tongKhach = 0;

    cout << "\n========== THONG KE NGAY "
         << ngay << " ==========\n";

    for (size_t i = 0; i < danhSach.size(); i++)
    {
        if (danhSach[i].getNgayDat() == ngay)
        {
            string trangThai = danhSach[i].getTrangThai();

            tong++;

            if (trangThai == "Da huy" ||
                trangThai == "Đã hủy" ||
                trangThai == "Huy" ||
                trangThai == "Hủy")
            {
                daHuy++;
            }
            else if (trangThai == "KhongDen" ||
                     trangThai == "Khong den" ||
                     trangThai == "Không đến")
            {
                khongDen++;
            }
            else
            {
                tongKhach += danhSach[i].getSoNguoi();
            }

            cout << "Ma dat ban: "
                 << danhSach[i].getMaDatBan() << '\n';
            cout << "Gio dat: "
                 << danhSach[i].getGioDat() << '\n';
            cout << "Trang thai: "
                 << trangThai << '\n';
            cout << "-----------------------------\n";
        }
    }

    if (tong == 0)
    {
        cout << "Khong co lich dat trong ngay nay!\n";
        return;
    }

    cout << "Tong luot dat: " << tong << '\n';
    cout << "So luot huy: " << daHuy << '\n';
    cout << "So luot khach khong den: "
         << khongDen << '\n';
    cout << "Tong so khach du kien (khong tinh huy/khong den): "
         << tongKhach << '\n';
}

void ThongKeDatBanService::thongKeKhungGio(
    const DatBanService& quanLyDatBan) const
{
    const vector<DatBan>& danhSach =
        quanLyDatBan.getDanhSachDatBan();

    int soLuong[24] = {};

    for (size_t i = 0; i < danhSach.size(); i++)
    {
        string trangThai = danhSach[i].getTrangThai();

        if (trangThai == "Da huy" ||
            trangThai == "Đã hủy" ||
            trangThai == "Huy" ||
            trangThai == "Hủy" ||
            trangThai == "KhongDen" ||
            trangThai == "Khong den" ||
            trangThai == "Không đến")
        {
            continue;
        }

        string gio = danhSach[i].getGioDat();

        if (gio.length() < 5 ||
            gio[2] != ':' ||
            gio[0] < '0' || gio[0] > '9' ||
            gio[1] < '0' || gio[1] > '9' ||
            gio[3] < '0' || gio[3] > '9' ||
            gio[4] < '0' || gio[4] > '9')
        {
            continue;
        }

        int gioSo = (gio[0] - '0') * 10 +
                    (gio[1] - '0');

        int phutSo = (gio[3] - '0') * 10 +
                     (gio[4] - '0');

        if (gioSo >= 0 && gioSo < 24 &&
            phutSo >= 0 && phutSo < 60)
        {
            soLuong[gioSo]++;
        }
    }

    int gioDongKhach = -1;
    int nhieuNhat = 0;

    cout << "\n========== THONG KE KHUNG GIO ==========\n";

    for (int i = 0; i < 24; i++)
    {
        if (soLuong[i] > 0)
        {
            cout << (i < 10 ? "0" : "") << i
                 << ":00 - "
                 << (i < 9 ? "0" : "") << i + 1
                 << ":00 : "
                 << soLuong[i] << " luot dat\n";
        }

        if (soLuong[i] > nhieuNhat)
        {
            nhieuNhat = soLuong[i];
            gioDongKhach = i;
        }
    }

    if (gioDongKhach == -1)
    {
        cout << "Chua co du lieu de thong ke!\n";
    }
    else
    {
        cout << "Khung gio co nhieu luot dat nhat: "
             << (gioDongKhach < 10 ? "0" : "")
             << gioDongKhach << ":00 - "
             << (gioDongKhach < 9 ? "0" : "")
             << gioDongKhach + 1 << ":00\n";

        cout << "So luot dat: " << nhieuNhat << '\n';
    }
}