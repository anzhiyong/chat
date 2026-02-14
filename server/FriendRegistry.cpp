#include "FriendRegistry.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

FriendRegistry::FriendRegistry()
{
    const QString baseDir = QDir(QCoreApplication::applicationDirPath()).filePath("data");
    QDir().mkpath(baseDir);
    m_filePath = QDir(baseDir).filePath("friends.json");
    loadFromDisk();
}

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
    saveToDisk();
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
    saveToDisk();
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

void FriendRegistry::loadFromDisk()
{
    m_friendMap.clear();

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
    const QJsonObject friendsObj = root.value("friends").toObject();
    for (auto it = friendsObj.constBegin(); it != friendsObj.constEnd(); ++it) {
        const QString user = it.key();
        const QJsonArray arr = it.value().toArray();
        for (const QJsonValue &v : arr) {
            const QString peer = v.toString();
            if (peer.isEmpty() || peer == user) {
                continue;
            }
            m_friendMap[user].insert(peer);
            m_friendMap[peer].insert(user);
        }
    }
}

void FriendRegistry::saveToDisk() const
{
    QJsonObject friendsObj;
    for (auto it = m_friendMap.constBegin(); it != m_friendMap.constEnd(); ++it) {
        QJsonArray arr;
        const QList<QString> peers = it.value().values();
        for (const QString &peer : peers) {
            arr.append(peer);
        }
        friendsObj.insert(it.key(), arr);
    }

    QJsonObject root;
    root.insert("friends", friendsObj);

    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
}
