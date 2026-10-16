#include "DatCocService.h"

bool DatCocService::themDatCoc(const DatCoc& datCoc)
{
    if (datCoc.getMaDatCoc().empty() ||
        datCoc.getMaDatBan().empty() ||
        datCoc.getTienCoc() <= 0 ||
        datCoc.getNgayDatCoc().empty())
    {
        cout << "Thong tin dat coc khong hop le!\n";
        return false;
    }

    for (size_t i = 0; i < danhSachDatCoc.size(); i++)
    {
        if (danhSachDatCoc[i].getMaDatCoc()
            == datCoc.getMaDatCoc())
        {
            cout << "Ma dat coc da ton tai!\n";
            return false;
        }

        if (danhSachDatCoc[i].getMaDatBan()
                == datCoc.getMaDatBan() &&
            danhSachDatCoc[i].getTrangThai() == "DaCoc")
        {
            cout << "Dat ban nay da co tien coc!\n";
            return false;
        }
    }

    danhSachDatCoc.push_back(datCoc);
    cout << "Them tien coc thanh cong!\n";
    return true;
}

bool DatCocService::hoanCoc(const string& maDatCoc)
{
    for (size_t i = 0; i < danhSachDatCoc.size(); i++)
    {
        if (danhSachDatCoc[i].getMaDatCoc() == maDatCoc)
        {
            if (danhSachDatCoc[i].getTrangThai() != "DaCoc")
            {
                cout << "Tien coc khong o trang thai co the hoan!\n";
                return false;
            }

            danhSachDatCoc[i].setTrangThai("DaHoan");
            cout << "Da ghi nhan hoan tien coc!\n";
            return true;
        }
    }

    cout << "Khong tim thay ma dat coc!\n";
    return false;
}

bool DatCocService::capNhatTienCoc(
    const string& maDatCoc, double tienMoi)
{
    if (tienMoi <= 0)
    {
        cout << "Tien coc phai lon hon 0!\n";
        return false;
    }

    for (size_t i = 0; i < danhSachDatCoc.size(); i++)
    {
        if (danhSachDatCoc[i].getMaDatCoc() == maDatCoc)
        {
            if (danhSachDatCoc[i].getTrangThai() != "DaCoc")
            {
                cout << "Khong the sua tien coc o trang thai nay!\n";
                return false;
            }

            danhSachDatCoc[i].setTienCoc(tienMoi);
            cout << "Cap nhat tien coc thanh cong!\n";
            return true;
        }
    }

    cout << "Khong tim thay ma dat coc!\n";
    return false;
}

void DatCocService::hienThiDanhSach() const
{
    if (danhSachDatCoc.empty())
    {
        cout << "Chua co du lieu dat coc!\n";
        return;
    }

    for (size_t i = 0; i < danhSachDatCoc.size(); i++)
    {
        cout << "\n--- Phieu dat coc " << i + 1 << " ---\n";
        danhSachDatCoc[i].hienThi();
    }
}

double DatCocService::tinhTongTienCoc() const
{
    double tong = 0;

    for (size_t i = 0; i < danhSachDatCoc.size(); i++)
    {
        if (danhSachDatCoc[i].getTrangThai() == "DaCoc")
        {
            tong += danhSachDatCoc[i].getTienCoc();
        }
    }

    return tong;
}

double DatCocService::tinhTienCoc(
    const string& maDatBan) const
{
    double tong = 0;

    for (size_t i = 0; i < danhSachDatCoc.size(); i++)
    {
        if (danhSachDatCoc[i].getMaDatBan() == maDatBan &&
            danhSachDatCoc[i].getTrangThai() == "DaCoc")
        {
            tong += danhSachDatCoc[i].getTienCoc();
        }
    }

    return tong;
}
