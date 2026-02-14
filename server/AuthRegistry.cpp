#include "AuthRegistry.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

AuthRegistry::AuthRegistry()
{
    const QString baseDir = QDir(QCoreApplication::applicationDirPath()).filePath("data");
    QDir().mkpath(baseDir);
    m_filePath = QDir(baseDir).filePath("accounts.json");
    loadFromDisk();
}

bool AuthRegistry::registerAccount(const QString &account, const QString &password, QString *errorText)
{
    const QString acc = account.trimmed();
    const QString pwd = password;

    if (acc.isEmpty() || pwd.isEmpty()) {
        if (errorText) {
            *errorText = "account_or_password_empty";
        }
        return false;
    }

    if (pwd.size() < 6) {
        if (errorText) {
            *errorText = "password_too_short";
        }
        return false;
    }

    if (m_passwordByAccount.contains(acc)) {
        if (errorText) {
            *errorText = "account_already_exists";
        }
        return false;
    }

    m_passwordByAccount.insert(acc, pwd);
    saveToDisk();
    return true;
}

bool AuthRegistry::validateLogin(const QString &account, const QString &password, QString *errorText) const
{
    const QString acc = account.trimmed();
    if (!m_passwordByAccount.contains(acc)) {
        if (errorText) {
            *errorText = "account_not_found";
        }
        return false;
    }

    if (m_passwordByAccount.value(acc) != password) {
        if (errorText) {
            *errorText = "wrong_password";
        }
        return false;
    }

    return true;
}

bool AuthRegistry::hasAccount(const QString &account) const
{
    return m_passwordByAccount.contains(account.trimmed());
}

void AuthRegistry::loadFromDisk()
{
    m_passwordByAccount.clear();

    QFile file(m_filePath);
    if (!file.exists()) {
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return;
    }

    const QJsonObject root = doc.object();
    const QJsonObject accounts = root.value("accounts").toObject();
    for (auto it = accounts.constBegin(); it != accounts.constEnd(); ++it) {
        m_passwordByAccount.insert(it.key(), it.value().toString());
    }
}

void AuthRegistry::saveToDisk() const
{
    QJsonObject accounts;
    for (auto it = m_passwordByAccount.constBegin(); it != m_passwordByAccount.constEnd(); ++it) {
        accounts.insert(it.key(), it.value());
    }

    QJsonObject root;
    root.insert("accounts", accounts);

    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
}
