
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

class DatabaseManager
{
public:
    static bool connect(QString* error = nullptr);
    static bool isOpen();
    static QSqlDatabase database();
    static void close();
};

#endif
