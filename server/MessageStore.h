#pragma once

#include <QJsonArray>
#include <QPair>
#include <QString>

/*
 * MessageStore = 私聊消息存储层（文件版）。
 *
 * 存储格式：
 * - NDJSON（每行一条 JSON）
 * - 文件路径：<server可执行目录>/data/private_messages.ndjson
 */
class MessageStore
{
public:
    MessageStore();

    bool appendPrivate(const QString &from, const QString &to, const QString &text, qint64 ts);
    QJsonArray loadPrivateHistory(const QString &user, const QString &peer, int limit) const;
    QPair<QString, qint64> lastPrivateSummary(const QString &user, const QString &peer) const;

private:
    QString m_filePath;
};

