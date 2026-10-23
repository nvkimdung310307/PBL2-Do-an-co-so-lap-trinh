#ifndef PASSWORDUTILS_H
#define PASSWORDUTILS_H

#include <QString>

namespace PasswordUtils {
    QString hashPassword(const QString& password);
    bool verifyPassword(const QString& password, const QString& storedValue);
    bool isPasswordHash(const QString& storedValue);
}

#endif
