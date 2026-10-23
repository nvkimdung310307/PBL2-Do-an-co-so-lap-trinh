#include "TachBanService.h"
#include "../DatabaseManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <iostream>

namespace {
QString qs(const std::string& s) { return QString::fromUtf8(s.data(), static_cast<int>(s.size())); }
void logSql(const char* action, const QSqlQuery& q) { std::cerr << action << ": " << q.lastError().text().toStdString() << '\n'; }
}
TachBanService::TachBanService(BanService& ban) : banService(ban) {}

bool TachBanService::tachBan(const string& maBanGoc, const string& maBanMoi)
{
    if (maBanGoc.empty() || maBanMoi.empty() || maBanGoc==maBanMoi) {
        std::cerr << "Ma ban goc/tach khong hop le hoac bi trung.\n"; return false;
    }
    if (!DatabaseManager::connect()) { std::cerr << "SQL Server chua ket noi.\n"; return false; }
    QSqlQuery q(DatabaseManager::database());
    q.prepare("EXEC dbo.sp_TachNhomBan @MaBanGoc=?, @MaBanTach=?");
    q.addBindValue(qs(maBanGoc)); q.addBindValue(qs(maBanMoi));
    if (!q.exec()) { logSql("Tach ban that bai",q); return false; }
    q.finish();
    (void)banService.getDanhSachBan(); // refresh the SQL-backed table cache after success
    std::cout << "Da tach ban " << maBanMoi << " khoi nhom ban co ban goc " << maBanGoc << ".\n";
    return true;
}
