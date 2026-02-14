#include "MessageStore.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

MessageStore::MessageStore()
{
    const QString baseDir = QDir(QCoreApplication::applicationDirPath()).filePath("data");
    QDir().mkpath(baseDir);
    m_filePath = QDir(baseDir).filePath("private_messages.ndjson");
}

bool MessageStore::appendPrivate(const QString &from, const QString &to, const QString &text, qint64 ts)
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        return false;
    }

    QJsonObject obj;
    obj["type"] = "private";
    obj["from"] = from;
    obj["to"] = to;
    obj["text"] = text;
    obj["ts"] = static_cast<double>(ts);

    QByteArray line = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    line.append('\n');
    return file.write(line) == line.size();
}

QJsonArray MessageStore::loadPrivateHistory(const QString &user, const QString &peer, int limit) const
{
    QJsonArray matched;
    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return matched;
    }

    while (!file.atEnd()) {
        const QByteArray line = file.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }

        QJsonParseError err;
        const QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
            continue;
        }

        const QJsonObject obj = doc.object();
        const QString from = obj.value("from").toString();
        const QString to = obj.value("to").toString();
        const bool related = (from == user && to == peer) || (from == peer && to == user);
        if (!related) {
            continue;
        }
        matched.append(obj);
    }

    if (limit > 0 && matched.size() > limit) {
        QJsonArray clipped;
        for (int i = matched.size() - limit; i < matched.size(); ++i) {
            clipped.append(matched.at(i));
        }
        return clipped;
    }
    return matched;
}

QPair<QString, qint64> MessageStore::lastPrivateSummary(const QString &user, const QString &peer) const
{
    QString lastText;
    qint64 lastTs = 0;

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return qMakePair(lastText, lastTs);
    }

    while (!file.atEnd()) {
        const QByteArray line = file.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }

        QJsonParseError err;
        const QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
            continue;
        }
        const QJsonObject obj = doc.object();

        const QString from = obj.value("from").toString();
        const QString to = obj.value("to").toString();
        const bool related = (from == user && to == peer) || (from == peer && to == user);
        if (!related) {
            continue;
        }

        const qint64 ts = static_cast<qint64>(obj.value("ts").toDouble(0));
        if (ts >= lastTs) {
            lastTs = ts;
            lastText = obj.value("text").toString();
        }
    }

    return qMakePair(lastText, lastTs);
}

