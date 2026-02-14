#include "FriendRegistry.h"

bool FriendRegistry::addFriend(const QString &a, const QString &b, QString *errorText)
{
    const QString u1 = a.trimmed();
    const QString u2 = b.trimmed();

    if (u1.isEmpty() || u2.isEmpty()) {
        if (errorText) {
            *errorText = "好友名不能为空";
        }
        return false;
    }

    if (u1 == u2) {
        if (errorText) {
            *errorText = "不能添加自己为好友";
        }
        return false;
    }

    if (m_friendMap.value(u1).contains(u2)) {
        if (errorText) {
            *errorText = QString("%1 已是你的好友").arg(u2);
        }
        return false;
    }

    m_friendMap[u1].insert(u2);
    m_friendMap[u2].insert(u1);
    return true;
}

bool FriendRegistry::removeFriend(const QString &a, const QString &b, QString *errorText)
{
    const QString u1 = a.trimmed();
    const QString u2 = b.trimmed();

    if (!m_friendMap.contains(u1) || !m_friendMap.value(u1).contains(u2)) {
        if (errorText) {
            *errorText = QString("%1 不是你的好友").arg(u2);
        }
        return false;
    }

    m_friendMap[u1].remove(u2);
    m_friendMap[u2].remove(u1);
    return true;
}

bool FriendRegistry::isFriend(const QString &a, const QString &b) const
{
    return m_friendMap.contains(a) && m_friendMap.value(a).contains(b);
}

QStringList FriendRegistry::friendsOf(const QString &user) const
{
    QStringList list = m_friendMap.value(user).values();
    list.sort(Qt::CaseInsensitive);
    return list;
}
