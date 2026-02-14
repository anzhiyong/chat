#include "GroupRegistry.h"

bool GroupRegistry::createGroup(const QString &group, const QString &owner, QString *errorText)
{
    if (group.trimmed().isEmpty()) {
        if (errorText) {
            *errorText = "群名不能为空";
        }
        return false;
    }

    if (m_groups.contains(group)) {
        if (errorText) {
            *errorText = QString("群 %1 已存在").arg(group);
        }
        return false;
    }

    GroupInfo info;
    info.owner = owner;
    info.members.insert(owner);
    m_groups.insert(group, info);
    return true;
}

bool GroupRegistry::joinGroup(const QString &group, const QString &user, QString *errorText)
{
    if (!m_groups.contains(group)) {
        if (errorText) {
            *errorText = QString("群 %1 不存在").arg(group);
        }
        return false;
    }

    m_groups[group].members.insert(user);
    return true;
}

bool GroupRegistry::leaveGroup(const QString &group, const QString &user, bool *groupDeleted, QString *errorText)
{
    if (groupDeleted) {
        *groupDeleted = false;
    }

    if (!m_groups.contains(group)) {
        if (errorText) {
            *errorText = QString("群 %1 不存在").arg(group);
        }
        return false;
    }

    GroupInfo &info = m_groups[group];
    if (!info.members.contains(user)) {
        if (errorText) {
            *errorText = QString("你不在群 %1 中").arg(group);
        }
        return false;
    }

    info.members.remove(user);

    if (info.owner == user) {
        if (info.members.isEmpty()) {
            m_groups.remove(group);
            if (groupDeleted) {
                *groupDeleted = true;
            }
            return true;
        }

        info.owner = *info.members.begin();
    }

    if (info.members.isEmpty()) {
        m_groups.remove(group);
        if (groupDeleted) {
            *groupDeleted = true;
        }
    }

    return true;
}

bool GroupRegistry::hasGroup(const QString &group) const
{
    return m_groups.contains(group);
}

bool GroupRegistry::isMember(const QString &group, const QString &user) const
{
    return m_groups.contains(group) && m_groups.value(group).members.contains(user);
}

QSet<QString> GroupRegistry::members(const QString &group) const
{
    return m_groups.contains(group) ? m_groups.value(group).members : QSet<QString>();
}

QString GroupRegistry::owner(const QString &group) const
{
    return m_groups.contains(group) ? m_groups.value(group).owner : QString();
}

QStringList GroupRegistry::groupsOfUser(const QString &user) const
{
    QStringList groups;
    for (auto it = m_groups.constBegin(); it != m_groups.constEnd(); ++it) {
        if (it.value().members.contains(user)) {
            groups.push_back(it.key());
        }
    }
    groups.sort(Qt::CaseInsensitive);
    return groups;
}

QStringList GroupRegistry::removeUserFromAllGroups(const QString &user)
{
    QStringList touchedGroups;

    for (auto it = m_groups.begin(); it != m_groups.end();) {
        GroupInfo &info = it.value();
        if (!info.members.contains(user)) {
            ++it;
            continue;
        }

        touchedGroups.push_back(it.key());
        info.members.remove(user);

        if (info.owner == user) {
            if (info.members.isEmpty()) {
                it = m_groups.erase(it);
                continue;
            }
            info.owner = *info.members.begin();
        }

        if (info.members.isEmpty()) {
            it = m_groups.erase(it);
            continue;
        }

        ++it;
    }

    return touchedGroups;
}
