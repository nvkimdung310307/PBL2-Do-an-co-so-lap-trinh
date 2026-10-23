#include "PasswordUtils.h"
#include <QStringList>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QByteArray>

namespace {
const int ITERATIONS = 150000;
const int KEY_LENGTH = 32;
const char* PREFIX = "pbkdf2-sha256";

QByteArray hmacSha256(const QByteArray& key, const QByteArray& message)
{
    const int blockSize = 64;
    QByteArray k = key;
    if (k.size() > blockSize)
        k = QCryptographicHash::hash(k, QCryptographicHash::Sha256);
    if (k.size() < blockSize)
        k.append(QByteArray(blockSize - k.size(), '\0'));

    QByteArray inner(blockSize, '\0');
    QByteArray outer(blockSize, '\0');
    for (int i = 0; i < blockSize; ++i) {
        inner[i] = static_cast<char>(k.at(i) ^ 0x36);
        outer[i] = static_cast<char>(k.at(i) ^ 0x5c);
    }
    const QByteArray innerHash = QCryptographicHash::hash(inner + message, QCryptographicHash::Sha256);
    return QCryptographicHash::hash(outer + innerHash, QCryptographicHash::Sha256);
}

QByteArray pbkdf2(const QByteArray& password, const QByteArray& salt, int iterations)
{
    QByteArray block = salt;
    block.append(char(0)); block.append(char(0)); block.append(char(0)); block.append(char(1));
    QByteArray u = hmacSha256(password, block);
    QByteArray result = u;
    for (int i = 1; i < iterations; ++i) {
        u = hmacSha256(password, u);
        for (int j = 0; j < result.size(); ++j)
            result[j] = static_cast<char>(result.at(j) ^ u.at(j));
    }
    return result.left(KEY_LENGTH);
}

bool constantTimeEqual(const QByteArray& a, const QByteArray& b)
{
    if (a.size() != b.size()) return false;
    unsigned char diff = 0;
    for (int i = 0; i < a.size(); ++i)
        diff |= static_cast<unsigned char>(a.at(i) ^ b.at(i));
    return diff == 0;
}
}

namespace PasswordUtils {
QString hashPassword(const QString& password)
{
    QByteArray salt(16, '\0');
    for (int i = 0; i < salt.size(); i += 4) {
        const quint32 randomValue = QRandomGenerator::system()->generate();
        for (int j = 0; j < 4 && i + j < salt.size(); ++j)
            salt[i + j] = static_cast<char>((randomValue >> (j * 8)) & 0xff);
    }
    const QByteArray derived = pbkdf2(password.toUtf8(), salt, ITERATIONS);
    return QString("%1$%2$%3$%4")
        .arg(QString::fromLatin1(PREFIX))
        .arg(ITERATIONS)
        .arg(QString::fromLatin1(salt.toBase64()))
        .arg(QString::fromLatin1(derived.toBase64()));
}

bool isPasswordHash(const QString& storedValue)
{
    return storedValue.startsWith(QString::fromLatin1(PREFIX) + "$", Qt::CaseSensitive);
}

bool verifyPassword(const QString& password, const QString& storedValue)
{
    if (!isPasswordHash(storedValue))
        return password == storedValue; // Legacy password, upgraded after successful login.

    const QStringList parts = storedValue.split('$');
    if (parts.size() != 4 || parts[0] != QString::fromLatin1(PREFIX)) return false;
    bool ok = false;
    const int iterations = parts[1].toInt(&ok);
    if (!ok || iterations < 10000 || iterations > 1000000) return false;
    const QByteArray salt = QByteArray::fromBase64(parts[2].toLatin1());
    const QByteArray expected = QByteArray::fromBase64(parts[3].toLatin1());
    if (salt.size() < 8 || expected.size() != KEY_LENGTH) return false;
    const QByteArray actual = pbkdf2(password.toUtf8(), salt, iterations);
    return constantTimeEqual(actual, expected);
}
}
