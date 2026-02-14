#pragma once

#include <QHash>
#include <QSet>
#include <QString>
#include <QStringList>

/*
 * FriendRegistry = 好友关系仓储
 * - 管理双向好友关系
 * - 查询好友列表
 */
class FriendRegistry
{
public:
    bool addFriend(const QString &a, const QString &b, QString *errorText = nullptr);
    bool removeFriend(const QString &a, const QString &b, QString *errorText = nullptr);

    bool isFriend(const QString &a, const QString &b) const;
    QStringList friendsOf(const QString &user) const;

private:
    QHash<QString, QSet<QString>> m_friendMap;
};
