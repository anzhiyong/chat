#pragma once

#include <QHash>
#include <QSet>
#include <QString>
#include <QStringList>

/*
 * GroupRegistry = 服务端“群状态仓储层”。
 *
 * 职责：
 * - 管理群与成员关系
 * - 管理群主变更
 * - 提供只读查询接口给路由层
 *
 * 不做网络 IO，不依赖 socket，确保高内聚可测试。
 */
class GroupRegistry
{
public:
    bool createGroup(const QString &group, const QString &owner, QString *errorText = nullptr);
    bool joinGroup(const QString &group, const QString &user, QString *errorText = nullptr);
    bool leaveGroup(const QString &group, const QString &user, bool *groupDeleted = nullptr, QString *errorText = nullptr);

    bool hasGroup(const QString &group) const;
    bool isMember(const QString &group, const QString &user) const;
    QSet<QString> members(const QString &group) const;
    QString owner(const QString &group) const;
    QStringList groupsOfUser(const QString &user) const;

    // 用户断连时批量清理，返回受影响群列表。
    QStringList removeUserFromAllGroups(const QString &user);

private:
    struct GroupInfo {
        QString owner;
        QSet<QString> members;
    };

    QHash<QString, GroupInfo> m_groups;
};
