
#include "DatabaseManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>

namespace {
const QString CONNECTION_NAME = "restaurant_sqlserver";
}

bool DatabaseManager::connect(QString* error)
{
    if (QSqlDatabase::contains(CONNECTION_NAME)) {
        QSqlDatabase existing =
            QSqlDatabase::database(CONNECTION_NAME);

        if (existing.isOpen())
            return true;

        existing.close();
    }

    const QString configPath =
        QDir(QCoreApplication::applicationDirPath())
            .filePath("database.ini");

    if (!QFile::exists(configPath)) {
        if (error)
            *error = "Khong tim thay database.ini: " + configPath;
        return false;
    }

    QSettings settings(configPath, QSettings::IniFormat);
    settings.beginGroup("database");

    const QString driver =
        settings.value("driver",
                       "ODBC Driver 18 for SQL Server").toString();
    const QString server =
        settings.value("server",
                       "localhost\\SQLEXPRESS").toString();
    const QString dbName =
        settings.value("database",
                       "RestaurantManagement").toString();
    const bool trusted =
        settings.value("trustedConnection", true).toBool();
    const QString username =
        settings.value("username", "").toString();
    const QString password =
        settings.value("password", "").toString();

    settings.endGroup();

    if (!QSqlDatabase::isDriverAvailable("QODBC")) {
        if (error) {
            *error =
                "Qt khong tim thay driver QODBC.\n"
                "Can kiem tra Qt SQL va plugin sqldrivers.";
        }
        return false;
    }

    QSqlDatabase db;

    if (QSqlDatabase::contains(CONNECTION_NAME)) {
        db = QSqlDatabase::database(CONNECTION_NAME);
    } else {
        db = QSqlDatabase::addDatabase(
            "QODBC", CONNECTION_NAME);
    }

    QString connectionString =
        QString("DRIVER={%1};SERVER=%2;DATABASE=%3;")
            .arg(driver, server, dbName);

    if (trusted) {
        connectionString += "Trusted_Connection=yes;";
    } else {
        connectionString +=
            QString("UID=%1;PWD=%2;")
                .arg(username, password);
    }

    connectionString += "TrustServerCertificate=yes;";

    db.setDatabaseName(connectionString);

    if (!db.open()) {
        if (error) {
            *error =
                "Khong ket noi duoc SQL Server.\n"
                "Server: " + server +
                "\nDatabase: " + dbName +
                "\nChi tiet: " + db.lastError().text();
        }
        return false;
    }

    QSqlQuery query(db);

    if (!query.exec("SELECT DB_NAME()") || !query.next()) {
        if (error) {
            *error =
                "Mo ket noi thanh cong nhung truy van that bai: " +
                query.lastError().text();
        }

        db.close();
        return false;
    }

    return true;
}

bool DatabaseManager::isOpen()
{
    return QSqlDatabase::contains(CONNECTION_NAME)
        && QSqlDatabase::database(CONNECTION_NAME).isOpen();
}

QSqlDatabase DatabaseManager::database()
{
    return QSqlDatabase::database(CONNECTION_NAME);
}

void DatabaseManager::close()
{
    if (QSqlDatabase::contains(CONNECTION_NAME)) {
        QSqlDatabase::database(CONNECTION_NAME).close();
    }
}
