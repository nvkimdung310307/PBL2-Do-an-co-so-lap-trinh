#include <QCoreApplication>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

#include "DatabaseManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString error;

    qDebug() << "Dang ket noi SQL Server...";

    if (!DatabaseManager::connect(&error)) {
        qCritical().noquote() << error;
        return 1;
    }

    QSqlQuery query(DatabaseManager::database());

    if (!query.exec(
            "SELECT DB_NAME() AS TenDatabase, "
            "COUNT(*) AS SoBang "
            "FROM INFORMATION_SCHEMA.TABLES "
            "WHERE TABLE_TYPE = 'BASE TABLE'")) {
        qCritical() << query.lastError().text();
        DatabaseManager::close();
        return 1;
    }

    if (query.next()) {
        qDebug() << "KET NOI THANH CONG!";
        qDebug() << "Database:"
                 << query.value("TenDatabase").toString();
        qDebug() << "So bang:"
                 << query.value("SoBang").toInt();
    }

    DatabaseManager::close();
    return 0;
}
