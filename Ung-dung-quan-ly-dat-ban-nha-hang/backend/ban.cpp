#include "Ban.h"
#include <windows.h>

void mau(int m) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, m);
}

Ban::Ban() {
    maBan = 0;
    soCho = 0;
    coKhach = false;
}

Ban::Ban(int ma, int cho) {
    maBan = ma;
    soCho = cho;
    coKhach = false;
}

int Ban::getMaBan() {
    return maBan;
}

int Ban::getSoCho() {
    return soCho;
}

bool Ban::getCoKhach() {
    return coKhach;
}

void Ban::setSoCho(int cho) {
    soCho = cho;
}

void Ban::setCoKhach(bool trangThai) {
    coKhach = trangThai;
}

void Ban::nhanKhach() {
    coKhach = true;
}

void Ban::thanhToan() {
    coKhach = false;
}

void Ban::hienThi() {

    // Ban co khach -> mau do
    if (coKhach)
        mau(12);
    else
        // Ban trong -> mau xanh
        mau(10);

    cout << "+-------------+" << endl;
    cout << "|   BAN ";

    if (maBan < 10)
        cout << "0";

    cout << maBan << "    |" << endl;

    if (coKhach)
        cout << "|   CO KHACH  |" << endl;
    else
        cout << "|    TRONG    |" << endl;

    cout << "|   " << soCho << " CHO      |" << endl;
    cout << "+-------------+" << endl;

    // Tra lai mau mac dinh
    mau(7);
}
