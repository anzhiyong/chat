#include "AuthRegistry.h"

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

