#include "BaoCaoService.h"
#include "../DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <iostream>

using namespace std;

BaoCaoService::BaoCaoService(BanService& banService)
    : banService(banService) {
}

void BaoCaoService::thongKeBan() {
    if (!DatabaseManager::isOpen()) {
        cerr << "SQL Server chua ket noi; khong the thong ke ban.\n";
        return;
    }

    QSqlQuery q(DatabaseManager::database());
    const QString sql = QString::fromUtf8(
        "SELECT COUNT(*) AS TongBan, "
        "SUM(CASE WHEN TrangThai IN (N'Trống',N'Trong') THEN 1 ELSE 0 END) AS BanTrong, "
        "SUM(CASE WHEN TrangThai IN (N'Đang sử dụng',N'Dang su dung') THEN 1 ELSE 0 END) AS BanDangSuDung, "
        "SUM(CASE WHEN TrangThai IN (N'Đã đặt',N'Da dat') THEN 1 ELSE 0 END) AS BanDaDat, "
        "SUM(CASE WHEN TrangThai IN (N'Bảo trì',N'Bao tri') THEN 1 ELSE 0 END) AS BanBaoTri "
        "FROM dbo.Ban");

    if (!q.exec(sql)) {
        cerr << "Thong ke ban that bai: " << q.lastError().text().toStdString() << '\n';
        return;
    }
    if (!q.next()) {
        cerr << "Khong doc duoc ket qua thong ke ban.\n";
        return;
    }

    cout << "\n========== BAO CAO BAN (SQL SERVER) ==========\n"
         << "Tong so ban: " << q.value("TongBan").toInt() << '\n'
         << "Ban trong: " << q.value("BanTrong").toInt() << '\n'
         << "Ban dang su dung: " << q.value("BanDangSuDung").toInt() << '\n'
         << "Ban da dat: " << q.value("BanDaDat").toInt() << '\n'
         << "Ban bao tri: " << q.value("BanBaoTri").toInt() << '\n'
         << "==============================================\n";
}
