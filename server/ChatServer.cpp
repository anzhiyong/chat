#include "ChatServer.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTcpServer>
#include <QTcpSocket>

/*
 * ChatServer.cpp（连接管理与协议分发）
 * 本文件不实现具体业务规则，只负责：
 * - 连接生命周期
 * - TCP 拆包
 * - 根据 type 路由到 handler
 */

ChatServer::ChatServer(QObject *parent)
    : QObject(parent), m_server(new QTcpServer(this))
{
    connect(m_server, &QTcpServer::newConnection, this, &ChatServer::onNewConnection);
}

bool ChatServer::start(const QHostAddress &address, quint16 port)
{
    if (!m_server->listen(address, port)) {
        qCritical() << "Listen failed:" << m_server->errorString();
        return false;
    }

    qInfo() << "Chat server listening on" << m_server->serverAddress().toString() << ":" << m_server->serverPort();
    return true;
}

void ChatServer::onNewConnection()
{
    while (QTcpSocket *socket = m_server->nextPendingConnection()) {
        ClientInfo info;
        info.name = QString("guest-%1").arg(reinterpret_cast<quintptr>(socket) & 0xFFFF);
        m_clients.insert(socket, info);

        connect(socket, &QTcpSocket::readyRead, this, &ChatServer::onSocketReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &ChatServer::onSocketDisconnected);

        qInfo() << "Client connected:" << socket->peerAddress().toString() << ":" << socket->peerPort();
        sendSystem(socket, "欢迎连接到聊天室服务器");
    }
}

void ChatServer::onSocketReadyRead()
{
    auto *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket || !m_clients.contains(socket)) {
        return;
    }

    ClientInfo &info = m_clients[socket];
    info.buffer.append(socket->readAll());

    // 协议使用 NDJSON（一行一个 JSON）。
    int newlineIndex = info.buffer.indexOf('\n');
    while (newlineIndex >= 0) {
        const QByteArray line = info.buffer.left(newlineIndex).trimmed();
        info.buffer.remove(0, newlineIndex + 1);

        if (!line.isEmpty()) {
            processLine(socket, line);
        }

        newlineIndex = info.buffer.indexOf('\n');
    }
}

void ChatServer::onSocketDisconnected()
{
    auto *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket || !m_clients.contains(socket)) {
        return;
    }

    const QString name = m_clients.value(socket).name;
    m_clients.remove(socket);

    if (m_userToSocket.value(name) == socket) {
        m_userToSocket.remove(name);
    }
    m_userNicknames.remove(name);

    // 用户断开时，从所有群移除并通知相关群。
    const QStringList touchedGroups = m_groups.removeUserFromAllGroups(name);
    for (const QString &group : touchedGroups) {
        broadcastGroupSystem(group, QString("%1 离开群 %2").arg(name, group));
    }

    qInfo() << "Client disconnected:" << name;
    broadcastSystem(QString("%1 离开聊天室").arg(name));

    socket->deleteLater();
}

void ChatServer::processLine(QTcpSocket *socket, const QByteArray &line)
{
    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(line, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        sendSystem(socket, "收到非法消息（非 JSON）");
        return;
    }

    const QJsonObject obj = doc.object();
    const QString type = obj.value("type").toString();

    if (type == "login") {
        handleLogin(socket, obj);
        return;
    }
    if (type == "auth_register") {
        handleAuthRegister(socket, obj);
        return;
    }
    if (type == "auth_login") {
        handleAuthLogin(socket, obj);
        return;
    }
    if (type == "session_list") {
        handleSessionList(socket, obj);
        return;
    }

    if (type == "join") {
        handleJoin(socket, obj);
        return;
    }
    if (type == "private") {
        handlePrivate(socket, obj);
        return;
    }
    if (type == "group") {
        handleGroup(socket, obj);
        return;
    }
    if (type == "create_group") {
        handleCreateGroup(socket, obj);
        return;
    }
    if (type == "join_group") {
        handleJoinGroup(socket, obj);
        return;
    }
    if (type == "leave_group") {
        handleLeaveGroup(socket, obj);
        return;
    }
    if (type == "search_user") {
        handleSearchUser(socket, obj);
        return;
    }
    if (type == "friend_request_send") {
        handleFriendRequestSend(socket, obj);
        return;
    }
    if (type == "friend_request_reply") {
        handleFriendRequestReply(socket, obj);
        return;
    }
    if (type == "add_friend") {
        handleAddFriend(socket, obj);
        return;
    }
    if (type == "remove_friend") {
        handleRemoveFriend(socket, obj);
        return;
    }
    if (type == "list_friends") {
        handleListFriends(socket, obj);
        return;
    }

    // 兼容旧客户端：chat 视为 group(all)
    if (type == "chat") {
        QJsonObject compat = obj;
        compat["type"] = "group";
        if (!compat.contains("group")) {
            compat["group"] = "all";
        }
        handleGroup(socket, compat);
        return;
    }

    sendSystem(socket, QString("未知消息类型: %1").arg(type));
}

QString ChatServer::resolveSenderName(QTcpSocket *socket, const QJsonObject &obj) const
{
    const QString fromObj = obj.value("name").toString().trimmed();
    if (!fromObj.isEmpty()) {
        return fromObj;
    }
    return m_clients.value(socket).name;
}

void ChatServer::sendJson(QTcpSocket *socket, const QJsonObject &obj)
{
    if (!socket) {
        return;
    }

    QByteArray payload = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    payload.append('\n');
    socket->write(payload);
}

void ChatServer::sendAck(QTcpSocket *socket, const QString &requestId, bool ok, const QString &error)
{
    QJsonObject obj;
    obj["type"] = "ack";
    if (!requestId.isEmpty()) {
        obj["request_id"] = requestId;
    }
    obj["ok"] = ok;
    if (!ok && !error.isEmpty()) {
        obj["error"] = error;
    }
    sendJson(socket, obj);
}

void ChatServer::sendSystem(QTcpSocket *socket, const QString &text)
{
    QJsonObject obj;
    obj["type"] = "system";
    obj["text"] = text;
    sendJson(socket, obj);
}

void ChatServer::broadcastJson(const QJsonObject &obj)
{
    for (QTcpSocket *client : m_clients.keys()) {
        sendJson(client, obj);
    }
}

void ChatServer::broadcastSystem(const QString &text)
{
    QJsonObject obj;
    obj["type"] = "system";
    obj["text"] = text;
    broadcastJson(obj);
}

void ChatServer::broadcastGroupSystem(const QString &group, const QString &text)
{
    if (!m_groups.hasGroup(group)) {
        return;
    }

    QJsonObject obj;
    obj["type"] = "system";
    obj["text"] = text;

    const QSet<QString> members = m_groups.members(group);
    for (const QString &member : members) {
        QTcpSocket *peer = m_userToSocket.value(member, nullptr);
        if (peer) {
            sendJson(peer, obj);
        }
    }
}

void ChatServer::sendFriendList(QTcpSocket *socket, const QString &user)
{
    QJsonObject obj;
    obj["type"] = "friend_list";

    QJsonArray friendsArray;
    const QStringList friends = m_friends.friendsOf(user);
    for (const QString &name : friends) {
        friendsArray.append(name);
    }
    obj["friends"] = friendsArray;

    sendJson(socket, obj);
}
