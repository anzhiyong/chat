#include "ChatServer.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QTcpSocket>
#include <QtGlobal>

/*
 * ChatServerHandlers.cpp（业务处理模块）
 * 只处理协议语义：
 * - 登录
 * - 私聊（好友校验）
 * - 群聊
 * - 建群/加群/退群
 * - 好友搜索/添加/删除/列表
 */

void ChatServer::handleAuthRegister(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString requestId = obj.value("request_id").toString();
    const QString account = obj.value("account").toString().trimmed();
    const QString password = obj.value("password").toString();

    QString error;
    const bool ok = m_auth.registerAccount(account, password, &error);

    QJsonObject out;
    out["type"] = "auth_register_result";
    if (!requestId.isEmpty()) {
        out["request_id"] = requestId;
    }
    out["ok"] = ok;
    if (!ok) {
        out["error"] = error;
    }
    sendJson(socket, out);
}

void ChatServer::handleAuthLogin(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString requestId = obj.value("request_id").toString();
    const QString account = obj.value("account").toString().trimmed();
    const QString password = obj.value("password").toString();

    QString error;
    const bool ok = m_auth.validateLogin(account, password, &error);

    QJsonObject out;
    out["type"] = "auth_login_result";
    if (!requestId.isEmpty()) {
        out["request_id"] = requestId;
    }
    out["ok"] = ok;

    if (!ok) {
        out["error"] = error;
        sendJson(socket, out);
        return;
    }

    QTcpSocket *occupied = m_userToSocket.value(account, nullptr);
    if (occupied && occupied != socket) {
        out["ok"] = false;
        out["error"] = "account_already_online";
        sendJson(socket, out);
        return;
    }

    const QString oldName = m_clients.value(socket).name;
    if (m_userToSocket.value(oldName) == socket) {
        m_userToSocket.remove(oldName);
    }

    m_clients[socket].name = account;
    m_userToSocket.insert(account, socket);
    m_userNicknames.insert(account, account);

    out["user_id"] = account;
    sendJson(socket, out);
    sendFriendList(socket, account);
    qInfo() << "[auth_login] success user=" << account << "from"
            << socket->peerAddress().toString() << ":" << socket->peerPort();
}

void ChatServer::handleLogin(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString requestId = obj.value("request_id").toString();
    const QString userId = obj.value("user_id").toString().trimmed();
    const QString nickname = obj.value("nickname").toString().trimmed();

    if (userId.isEmpty()) {
        sendAck(socket, requestId, false, "missing_field:user_id");
        return;
    }

    QTcpSocket *occupied = m_userToSocket.value(userId, nullptr);
    if (occupied && occupied != socket) {
        sendAck(socket, requestId, false, "user_already_online");
        return;
    }

    const QString oldName = m_clients.value(socket).name;
    if (m_userToSocket.value(oldName) == socket) {
        m_userToSocket.remove(oldName);
    }

    // 协议里 user_id 是唯一身份，服务端统一把它当作会话主键。
    m_clients[socket].name = userId;
    m_userToSocket.insert(userId, socket);

    if (!nickname.isEmpty()) {
        m_userNicknames.insert(userId, nickname);
    } else if (!m_userNicknames.contains(userId)) {
        m_userNicknames.insert(userId, userId);
    }

    sendAck(socket, requestId, true);
    sendFriendList(socket, userId);
    qInfo() << "[login] success user=" << userId << "from"
            << socket->peerAddress().toString() << ":" << socket->peerPort();
}

void ChatServer::handleSessionList(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString requestId = obj.value("request_id").toString();
    const QString userId = obj.value("user_id").toString().trimmed();
    const QString effectiveUser = userId.isEmpty() ? m_clients.value(socket).name : userId;

    if (effectiveUser.isEmpty()) {
        sendAck(socket, requestId, false, "missing_field:user_id");
        return;
    }

    QJsonArray sessions;

    const QStringList friends = m_friends.friendsOf(effectiveUser);
    for (const QString &friendId : friends) {
        const QPair<QString, qint64> summary = m_messages.lastPrivateSummary(effectiveUser, friendId);
        QJsonObject item;
        item["session_type"] = "direct";
        item["session_id"] = friendId;
        item["title"] = m_userNicknames.value(friendId, friendId);
        item["last_text"] = summary.first;
        item["last_ts"] = static_cast<double>(summary.second);
        item["unread"] = 0;
        sessions.append(item);
    }

    const QStringList groups = m_groups.groupsOfUser(effectiveUser);
    for (const QString &groupId : groups) {
        QJsonObject item;
        item["session_type"] = "group";
        item["session_id"] = groupId;
        item["title"] = groupId;
        item["last_text"] = "";
        item["last_ts"] = 0;
        item["unread"] = 0;
        sessions.append(item);
    }

    QJsonObject out;
    out["type"] = "session_list_result";
    out["request_id"] = requestId;
    out["ok"] = true;
    out["sessions"] = sessions;
    sendJson(socket, out);
}

void ChatServer::handleHistoryPull(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString peerId = obj.value("peer_id").toString().trimmed();
    const int limit = obj.value("limit").toInt(50);
    const QString self = m_clients.value(socket).name;

    if (self.isEmpty()) {
        sendSystem(socket, "请先登录后再拉取历史消息");
        return;
    }
    if (peerId.isEmpty()) {
        sendSystem(socket, "history_pull 缺少 peer_id");
        return;
    }
    if (!m_friends.isFriend(self, peerId)) {
        sendSystem(socket, QString("你和 %1 不是好友，无法查看会话历史").arg(peerId));
        return;
    }

    QJsonObject out;
    out["type"] = "history_result";
    out["peer_id"] = peerId;
    out["history"] = m_messages.loadPrivateHistory(self, peerId, qMax(1, limit));
    sendJson(socket, out);
}

void ChatServer::handleJoin(QTcpSocket *socket, const QJsonObject &obj)
{
    QString requested = obj.value("name").toString().trimmed();
    if (requested.isEmpty()) {
        requested = m_clients.value(socket).name;
    }

    if (m_userToSocket.contains(requested) && m_userToSocket.value(requested) != socket) {
        sendSystem(socket, QString("用户名 %1 已被占用").arg(requested));
        return;
    }

    const QString oldName = m_clients.value(socket).name;
    if (m_userToSocket.value(oldName) == socket) {
        m_userToSocket.remove(oldName);
    }

    m_clients[socket].name = requested;
    m_userToSocket.insert(requested, socket);

    if (!m_groups.hasGroup("all")) {
        QString err;
        m_groups.createGroup("all", requested, &err);
    }

    QString joinErr;
    m_groups.joinGroup("all", requested, &joinErr);

    sendSystem(socket, QString("登录成功，当前用户名: %1").arg(requested));
    sendFriendList(socket, requested);
    broadcastSystem(QString("%1 加入聊天室").arg(requested));
}

void ChatServer::handlePrivate(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString sender = resolveSenderName(socket, obj);
    const QString to = obj.value("to").toString().trimmed();
    const QString text = obj.value("text").toString().trimmed();

    if (to.isEmpty() || text.isEmpty()) {
        sendSystem(socket, "私聊消息缺少 to 或 text");
        return;
    }

    if (!m_userToSocket.contains(to)) {
        sendSystem(socket, QString("用户 %1 不在线").arg(to));
        return;
    }

    if (!m_friends.isFriend(sender, to)) {
        sendSystem(socket, QString("你和 %1 不是好友，无法私聊").arg(to));
        return;
    }
    const qint64 ts = QDateTime::currentSecsSinceEpoch();
    m_messages.appendPrivate(sender, to, text, ts);

    QJsonObject out;
    out["type"] = "private";
    out["name"] = sender;
    out["to"] = to;
    out["text"] = text;
    out["ts"] = static_cast<double>(ts);
    out["time"] = QDateTime::fromSecsSinceEpoch(ts).toString(Qt::ISODate);

    sendJson(m_userToSocket.value(to), out);
    sendSystem(socket, QString("私聊消息已发送给 %1").arg(to));
}

void ChatServer::handleGroup(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString sender = resolveSenderName(socket, obj);
    const QString group = obj.value("group").toString("all").trimmed();
    const QString text = obj.value("text").toString().trimmed();

    if (group.isEmpty() || text.isEmpty()) {
        sendSystem(socket, "群消息缺少 group 或 text");
        return;
    }

    if (!m_groups.hasGroup(group)) {
        sendSystem(socket, QString("群 %1 不存在").arg(group));
        return;
    }

    if (!m_groups.isMember(group, sender)) {
        sendSystem(socket, QString("你不在群 %1 中，请先加群").arg(group));
        return;
    }

    QJsonObject out;
    out["type"] = "group";
    out["group"] = group;
    out["name"] = sender;
    out["text"] = text;
    out["time"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    const QSet<QString> members = m_groups.members(group);
    for (const QString &member : members) {
        QTcpSocket *peer = m_userToSocket.value(member, nullptr);
        if (peer) {
            sendJson(peer, out);
        }
    }
}

void ChatServer::handleCreateGroup(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString user = resolveSenderName(socket, obj);
    const QString group = obj.value("group").toString().trimmed();

    QString err;
    if (!m_groups.createGroup(group, user, &err)) {
        sendSystem(socket, err);
        return;
    }

    sendSystem(socket, QString("创建群 %1 成功").arg(group));
    broadcastGroupSystem(group, QString("%1 创建了群 %2").arg(user, group));
}

void ChatServer::handleJoinGroup(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString user = resolveSenderName(socket, obj);
    const QString group = obj.value("group").toString().trimmed();

    QString err;
    if (!m_groups.joinGroup(group, user, &err)) {
        sendSystem(socket, err);
        return;
    }

    sendSystem(socket, QString("已加入群 %1").arg(group));
    broadcastGroupSystem(group, QString("%1 加入群 %2").arg(user, group));
}

void ChatServer::handleLeaveGroup(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString user = resolveSenderName(socket, obj);
    const QString group = obj.value("group").toString().trimmed();

    bool deleted = false;
    QString err;
    if (!m_groups.leaveGroup(group, user, &deleted, &err)) {
        sendSystem(socket, err);
        return;
    }

    sendSystem(socket, QString("已退出群 %1").arg(group));
    if (deleted) {
        broadcastSystem(QString("群 %1 已解散（无成员）").arg(group));
    } else {
        broadcastGroupSystem(group, QString("%1 离开群 %2").arg(user, group));
    }
}

void ChatServer::handleSearchUser(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString self = resolveSenderName(socket, obj);
    const QString keyword = obj.value("keyword").toString().trimmed();

    QJsonObject out;
    out["type"] = "friend_search_result";
    out["keyword"] = keyword;

    QJsonArray users;
    for (const QString &name : m_userToSocket.keys()) {
        if (name == self) {
            continue;
        }
        if (keyword.isEmpty() || name.contains(keyword, Qt::CaseInsensitive)) {
            users.append(name);
        }
    }
    out["users"] = users;

    sendJson(socket, out);
}

void ChatServer::handleFriendRequestSend(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString from = m_clients.value(socket).name;
    const QString to = obj.value("to").toString().trimmed();
    const QString remark = obj.value("remark").toString().trimmed();

    if (from.isEmpty()) {
        sendSystem(socket, "请先登录后再发送好友申请");
        return;
    }
    if (to.isEmpty()) {
        sendSystem(socket, "好友申请缺少目标账号");
        return;
    }
    if (from == to) {
        sendSystem(socket, "不能添加自己为好友");
        return;
    }
    if (!m_auth.hasAccount(to)) {
        sendSystem(socket, QString("账号 %1 不存在").arg(to));
        return;
    }
    if (m_friends.isFriend(from, to)) {
        sendSystem(socket, QString("%1 已经是你的好友").arg(to));
        return;
    }

    QTcpSocket *toSocket = m_userToSocket.value(to, nullptr);
    if (!toSocket) {
        sendSystem(socket, QString("%1 当前不在线，暂时无法发送请求").arg(to));
        return;
    }

    QString pendingId;
    for (auto it = m_friendRequests.constBegin(); it != m_friendRequests.constEnd(); ++it) {
        const FriendRequest &req = it.value();
        if (!req.handled && req.from == from && req.to == to) {
            pendingId = req.id;
            break;
        }
    }
    if (!pendingId.isEmpty()) {
        sendSystem(socket, "你已经发送过好友申请，请等待对方处理");
        return;
    }

    FriendRequest req;
    req.id = QString("fr-%1").arg(m_friendRequestSeq++);
    req.from = from;
    req.to = to;
    req.remark = remark;
    m_friendRequests.insert(req.id, req);

    QJsonObject push;
    push["type"] = "friend_request_received";
    push["request_id"] = req.id;
    push["from"] = from;
    push["remark"] = req.remark;
    sendJson(toSocket, push);

    sendSystem(socket, QString("已向 %1 发送好友申请").arg(to));
}

void ChatServer::handleFriendRequestReply(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString me = m_clients.value(socket).name;
    const QString requestId = obj.value("request_id").toString().trimmed();
    const bool accept = obj.value("accept").toBool(false);

    if (me.isEmpty()) {
        sendSystem(socket, "请先登录后再处理好友申请");
        return;
    }
    if (requestId.isEmpty() || !m_friendRequests.contains(requestId)) {
        sendSystem(socket, "好友申请不存在或已失效");
        return;
    }

    FriendRequest &req = m_friendRequests[requestId];
    if (req.handled) {
        sendSystem(socket, "该好友申请已处理");
        return;
    }
    if (req.to != me) {
        sendSystem(socket, "你无权处理该好友申请");
        return;
    }

    req.handled = true;

    QTcpSocket *fromSocket = m_userToSocket.value(req.from, nullptr);
    QJsonObject notify;
    notify["type"] = "friend_request_result";
    notify["request_id"] = req.id;
    notify["from"] = req.from;
    notify["to"] = req.to;
    notify["accepted"] = accept;

    if (!accept) {
        if (fromSocket) {
            sendJson(fromSocket, notify);
        }
        sendSystem(socket, QString("你已拒绝 %1 的好友申请").arg(req.from));
        return;
    }

    QString err;
    if (!m_friends.addFriend(req.from, req.to, &err)) {
        sendSystem(socket, err);
        return;
    }

    if (fromSocket) {
        sendFriendList(fromSocket, req.from);
        sendJson(fromSocket, notify);
        sendSystem(fromSocket, QString("%1 通过了你的好友申请").arg(req.to));
    }
    sendFriendList(socket, req.to);
    sendSystem(socket, QString("你已通过 %1 的好友申请").arg(req.from));
}

void ChatServer::handleAddFriend(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString self = resolveSenderName(socket, obj);
    const QString peer = obj.value("friend").toString().trimmed();

    if (!m_userToSocket.contains(peer)) {
        sendSystem(socket, QString("用户 %1 不在线，暂时无法添加").arg(peer));
        return;
    }

    QString err;
    if (!m_friends.addFriend(self, peer, &err)) {
        sendSystem(socket, err);
        return;
    }

    sendSystem(socket, QString("已添加 %1 为好友").arg(peer));
    sendFriendList(socket, self);

    QTcpSocket *peerSocket = m_userToSocket.value(peer, nullptr);
    if (peerSocket) {
        sendSystem(peerSocket, QString("%1 添加了你为好友").arg(self));
        sendFriendList(peerSocket, peer);
    }
}

void ChatServer::handleRemoveFriend(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString self = resolveSenderName(socket, obj);
    const QString peer = obj.value("friend").toString().trimmed();

    QString err;
    if (!m_friends.removeFriend(self, peer, &err)) {
        sendSystem(socket, err);
        return;
    }

    sendSystem(socket, QString("已删除好友 %1").arg(peer));
    sendFriendList(socket, self);

    QTcpSocket *peerSocket = m_userToSocket.value(peer, nullptr);
    if (peerSocket) {
        sendSystem(peerSocket, QString("%1 已将你从好友中删除").arg(self));
        sendFriendList(peerSocket, peer);
    }
}

void ChatServer::handleListFriends(QTcpSocket *socket, const QJsonObject &obj)
{
    const QString self = resolveSenderName(socket, obj);
    sendFriendList(socket, self);
}
