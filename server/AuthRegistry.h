#pragma once

#include <QHash>
#include <QString>

/*
 * AuthRegistry = 账号认证仓储（内存版）。
 *
 * 说明：
 * - 本版本只做学习用途，数据保存在内存中，服务端重启后会丢失。
 * - 后续可以替换为数据库实现（MySQL/SQLite/Redis）。
 */
class AuthRegistry
{
public:
    AuthRegistry();

    bool registerAccount(const QString &account, const QString &password, QString *errorText = nullptr);
    bool validateLogin(const QString &account, const QString &password, QString *errorText = nullptr) const;
    bool hasAccount(const QString &account) const;

private:
    void loadFromDisk();
    void saveToDisk() const;

    QString m_filePath;
    QHash<QString, QString> m_passwordByAccount;
};
