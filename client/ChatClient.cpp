#include "ChatClient.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

ChatClient::ChatClient(QObject *parent)
    : QObject(parent), m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::connected, this, &ChatClient::onSocketConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ChatClient::onSocketDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &ChatClient::onReadyRead);
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &ChatClient::onSocketError);
}

void ChatClient::setUserName(const QString &name)
{
    m_userName = name.trimmed();
}

QString ChatClient::userName() const
{
    return m_userName;
}

void ChatClient::connectToServer(const QString &host, quint16 port)
{
    m_socket->connectToHost(host, port);
}

void ChatClient::disconnectFromServer()
{
    m_socket->disconnectFromHost();
}

bool ChatClient::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void ChatClient::sendJoin()
{
    QJsonObject obj;
    obj["type"] = "join";
    obj["name"] = m_userName;
    sendJson(obj);
}

void ChatClient::sendPrivate(const QString &to, const QString &text)
{
    QJsonObject obj;
    obj["type"] = "private";
    obj["name"] = m_userName;
    obj["to"] = to;
    obj["text"] = text;
    obj["time"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    sendJson(obj);
}

void ChatClient::sendGroup(const QString &group, const QString &text)
{
    QJsonObject obj;
    obj["type"] = "group";
    obj["name"] = m_userName;
    obj["group"] = group;
    obj["text"] = text;
    obj["time"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    sendJson(obj);
}

void ChatClient::sendCreateGroup(const QString &group)
{
    QJsonObject obj;
    obj["type"] = "create_group";
    obj["name"] = m_userName;
    obj["group"] = group;
    sendJson(obj);
}

void ChatClient::sendJoinGroup(const QString &group)
{
    QJsonObject obj;
    obj["type"] = "join_group";
    obj["name"] = m_userName;
    obj["group"] = group;
    sendJson(obj);
}

void ChatClient::sendLeaveGroup(const QString &group)
{
    QJsonObject obj;
    obj["type"] = "leave_group";
    obj["name"] = m_userName;
    obj["group"] = group;
    sendJson(obj);
}

void ChatClient::sendSearchUser(const QString &keyword)
{
    QJsonObject obj;
    obj["type"] = "search_user";
    obj["name"] = m_userName;
    obj["keyword"] = keyword.trimmed();
    sendJson(obj);
}

void ChatClient::sendAddFriend(const QString &friendName)
{
    QJsonObject obj;
    obj["type"] = "add_friend";
    obj["name"] = m_userName;
    obj["friend"] = friendName.trimmed();
    sendJson(obj);
}

void ChatClient::sendRemoveFriend(const QString &friendName)
{
    QJsonObject obj;
    obj["type"] = "remove_friend";
    obj["name"] = m_userName;
    obj["friend"] = friendName.trimmed();
    sendJson(obj);
}

void ChatClient::sendListFriends()
{
    QJsonObject obj;
    obj["type"] = "list_friends";
    obj["name"] = m_userName;
    sendJson(obj);
}

void ChatClient::onReadyRead()
{
    m_receiveBuffer.append(m_socket->readAll());

    int newlineIndex = m_receiveBuffer.indexOf('\n');
    while (newlineIndex >= 0) {
        const QByteArray line = m_receiveBuffer.left(newlineIndex).trimmed();
        m_receiveBuffer.remove(0, newlineIndex + 1);

        if (!line.isEmpty()) {
            processLine(line);
        }

        newlineIndex = m_receiveBuffer.indexOf('\n');
    }
}

void ChatClient::onSocketConnected()
{
    emit connected();
}

void ChatClient::onSocketDisconnected()
{
    emit disconnected();
}

void ChatClient::onSocketError()
{
    emit networkError(m_socket->errorString());
}

void ChatClient::sendJson(const QJsonObject &obj)
{
    QByteArray payload = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    payload.append('\n');
    m_socket->write(payload);
}

void ChatClient::processLine(const QByteArray &line)
{
    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(line, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        emit systemMessage(QString::fromUtf8(line));
        return;
    }

    const QJsonObject obj = doc.object();
    const QString type = obj.value("type").toString();

    if (type == "system") {
        emit systemMessage(obj.value("text").toString());
        return;
    }

    if (type == "private") {
        emit chatMessage(obj.value("name").toString("unknown"), obj.value("text").toString(), "private",
                         obj.value("to").toString());
        return;
    }

    if (type == "group") {
        emit chatMessage(obj.value("name").toString("unknown"), obj.value("text").toString(), "group",
                         obj.value("group").toString("all"));
        return;
    }

    if (type == "chat") {
        emit chatMessage(obj.value("name").toString("unknown"), obj.value("text").toString(), "group", "all");
        return;
    }

    if (type == "friend_search_result") {
        QStringList users;
        const QJsonArray array = obj.value("users").toArray();
        for (const QJsonValue &v : array) {
            users.push_back(v.toString());
        }
        emit friendSearchResult(users);
        return;
    }

    if (type == "friend_list") {
        QStringList friends;
        const QJsonArray array = obj.value("friends").toArray();
        for (const QJsonValue &v : array) {
            friends.push_back(v.toString());
        }
        emit friendListUpdated(friends);
        return;
    }

    emit systemMessage(QString::fromUtf8(line));
}
