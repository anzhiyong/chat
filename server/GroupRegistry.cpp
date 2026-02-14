#include "GroupRegistry.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

GroupRegistry::GroupRegistry()
{
    const QString baseDir = QDir(QCoreApplication::applicationDirPath()).filePath("data");
    QDir().mkpath(baseDir);
    m_filePath = QDir(baseDir).filePath("groups.json");
    loadFromDisk();
}

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
    saveToDisk();
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
    saveToDisk();
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
            saveToDisk();
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

    saveToDisk();
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
    bool changed = false;

    for (auto it = m_groups.begin(); it != m_groups.end();) {
        GroupInfo &info = it.value();
        if (!info.members.contains(user)) {
            ++it;
            continue;
        }

        touchedGroups.push_back(it.key());
        info.members.remove(user);
        changed = true;

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

    if (changed) {
        saveToDisk();
    }
    return touchedGroups;
}

void GroupRegistry::loadFromDisk()
{
    m_groups.clear();

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
    const QJsonObject groupsObj = root.value("groups").toObject();
    for (auto it = groupsObj.constBegin(); it != groupsObj.constEnd(); ++it) {
        const QString groupName = it.key();
        const QJsonObject obj = it.value().toObject();

        GroupInfo info;
        info.owner = obj.value("owner").toString();
        const QJsonArray members = obj.value("members").toArray();
        for (const QJsonValue &v : members) {
            const QString user = v.toString();
            if (!user.isEmpty()) {
                info.members.insert(user);
            }
        }

        if (info.owner.isEmpty()) {
            if (!info.members.isEmpty()) {
                info.owner = *info.members.begin();
            } else {
                continue;
            }
        }
        info.members.insert(info.owner);
        m_groups.insert(groupName, info);
    }
}

void GroupRegistry::saveToDisk() const
{
    QJsonObject groupsObj;
    for (auto it = m_groups.constBegin(); it != m_groups.constEnd(); ++it) {
        const GroupInfo &info = it.value();

        QJsonArray members;
        const QList<QString> memberList = info.members.values();
        for (const QString &m : memberList) {
            members.append(m);
        }

        QJsonObject groupObj;
        groupObj.insert("owner", info.owner);
        groupObj.insert("members", members);
        groupsObj.insert(it.key(), groupObj);
    }

    QJsonObject root;
    root.insert("groups", groupsObj);

    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
}
