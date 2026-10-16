
#include "BanService.h"
#include <iostream>

using namespace std;

// 1. Them ban
bool BanService::themBan(const Ban& ban)
{
    if (!ban.hopLe())
    {
        cout << "Thong tin ban khong hop le!\n";
        return false;
    }

    if (timBan(ban.getMaBan()) != nullptr)
    {
        cout << "Ma ban da ton tai!\n";
        return false;
    }

    danhSachBan.push_back(ban);

    cout << "Them ban thanh cong!\n";
    return true;
}

// 2. Sua thong tin ban
bool BanService::suaBan(
    const string& maBan,
    const Ban& banMoi)
{
    for (size_t i = 0; i < danhSachBan.size(); i++)
    {
        if (danhSachBan[i].getMaBan() == maBan)
        {
            // Khong cho doi ma ban
            Ban banCapNhat(
                maBan,
                banMoi.getTenBan(),
                banMoi.getSoCho(),
                banMoi.getTrangThai(),
                banMoi.getMaKhuVuc()
            );

            if (!banCapNhat.hopLe())
            {
                cout << "Thong tin moi khong hop le!\n";
                return false;
            }

            danhSachBan[i] = banCapNhat;

            cout << "Sua ban thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay ban can sua!\n";
    return false;
}

// 3. Xoa ban
bool BanService::xoaBan(const string& maBan)
{
    for (size_t i = 0; i < danhSachBan.size(); i++)
    {
        if (danhSachBan[i].getMaBan() == maBan)
        {
            string trangThai = danhSachBan[i].getTrangThai();

            if (trangThai != "Trống" && trangThai != "Trong")
            {
                cout << "Chi duoc xoa ban dang trong!\n";
                return false;
            }

            danhSachBan.erase(danhSachBan.begin() + i);

            cout << "Xoa ban thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay ban can xoa!\n";
    return false;
}

// 4. Tim ban theo ma
const Ban* BanService::timBan(const string& maBan) const
{
    for (size_t i = 0; i < danhSachBan.size(); i++)
    {
        if (danhSachBan[i].getMaBan() == maBan)
        {
            return &danhSachBan[i];
        }
    }

    return nullptr;
}

// 5. Hien thi danh sach ban
void BanService::hienThiDanhSach() const
{
    if (danhSachBan.empty())
    {
        cout << "Danh sach ban dang rong!\n";
        return;
    }

    cout << "\n========== DANH SACH BAN ==========\n";

    for (size_t i = 0; i < danhSachBan.size(); i++)
    {
        cout << "Ma ban: " << danhSachBan[i].getMaBan() << '\n';
        cout << "Ten ban: " << danhSachBan[i].getTenBan() << '\n';
        cout << "So cho: " << danhSachBan[i].getSoCho() << '\n';
        cout << "Trang thai: " << danhSachBan[i].getTrangThai() << '\n';
        cout << "Ma khu vuc: " << danhSachBan[i].getMaKhuVuc() << '\n';
        cout << "-----------------------------------\n";
    }
}

// 6. Tim ban trong theo so cho toi thieu
void BanService::timBanTrong(int soChoToiThieu) const
{
    bool timThay = false;

    if (soChoToiThieu <= 0)
    {
        cout << "So cho toi thieu phai lon hon 0!\n";
        return;
    }

    for (size_t i = 0; i < danhSachBan.size(); i++)
    {
        string trangThai = danhSachBan[i].getTrangThai();

        if ((trangThai == "Trống" || trangThai == "Trong")
            && danhSachBan[i].getSoCho() >= soChoToiThieu)
        {
            cout << "Ma ban: " << danhSachBan[i].getMaBan()
                 << " | Ten ban: " << danhSachBan[i].getTenBan()
                 << " | So cho: " << danhSachBan[i].getSoCho()
                 << '\n';

            timThay = true;
        }
    }

    if (!timThay)
    {
        cout << "Khong co ban trong phu hop!\n";
    }
}

// 7. Cap nhat trang thai ban
bool BanService::capNhatTrangThai(
    const string& maBan,
    const string& trangThai)
{
    for (size_t i = 0; i < danhSachBan.size(); i++)
    {
        if (danhSachBan[i].getMaBan() == maBan)
        {
            Ban banCapNhat(
                danhSachBan[i].getMaBan(),
                danhSachBan[i].getTenBan(),
                danhSachBan[i].getSoCho(),
                trangThai,
                danhSachBan[i].getMaKhuVuc()
            );

            if (!banCapNhat.hopLe())
            {
                cout << "Trang thai khong hop le!\n";
                return false;
            }

            danhSachBan[i].setTrangThai(trangThai);

            cout << "Cap nhat trang thai thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay ban!\n";
    return false;
}

// 8. Lay danh sach ban
const vector<Ban>& BanService::getDanhSachBan() const
{
    return danhSachBan;
}
// Tim ban trong theo so khach va khu vuc
void BanService::timBanTheoKhuVuc(
    int soChoToiThieu,
    const string& maKhuVuc) const
{
    if (soChoToiThieu <= 0)
    {
        cout << "So khach phai lon hon 0!\n";
        return;
    }

    bool timThay = false;

    for (size_t i = 0; i < danhSachBan.size(); i++)
    {
        string trangThai = danhSachBan[i].getTrangThai();

        if ((trangThai == "Trong" || trangThai == "Trống") &&
            danhSachBan[i].getSoCho() >= soChoToiThieu &&
            danhSachBan[i].getMaKhuVuc() == maKhuVuc)
        {
            cout << "Ma ban: " << danhSachBan[i].getMaBan()
                 << " | Ten ban: " << danhSachBan[i].getTenBan()
                 << " | So cho: " << danhSachBan[i].getSoCho()
                 << " | Khu vuc: " << danhSachBan[i].getMaKhuVuc()
                 << '\n';

            timThay = true;
        }
    }

    if (!timThay)
    {
        cout << "Khong co ban trong phu hop voi so khach va khu vuc!\n";
    }
}
