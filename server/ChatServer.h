#pragma once

#include <QHash>
#include <QObject>

#include "FriendRegistry.h"
#include "GroupRegistry.h"
#include "AuthRegistry.h"

class QHostAddress;
class QJsonObject;
class QTcpServer;
class QTcpSocket;

class ChatServer : public QObject
{
    Q_OBJECT

public:
    explicit ChatServer(QObject *parent = nullptr);
    bool start(const QHostAddress &address, quint16 port);

private slots:
    void onNewConnection();
    void onSocketReadyRead();
    void onSocketDisconnected();

private:
    struct ClientInfo {
        QString name;
        QByteArray buffer;
    };
    struct FriendRequest {
        QString id;
        QString from;
        QString to;
        QString remark;
        bool handled = false;
    };

    void processLine(QTcpSocket *socket, const QByteArray &line);

    void handleLogin(QTcpSocket *socket, const QJsonObject &obj);
    void handleAuthRegister(QTcpSocket *socket, const QJsonObject &obj);
    void handleAuthLogin(QTcpSocket *socket, const QJsonObject &obj);
    void handleSessionList(QTcpSocket *socket, const QJsonObject &obj);

    void handleJoin(QTcpSocket *socket, const QJsonObject &obj);
    void handlePrivate(QTcpSocket *socket, const QJsonObject &obj);
    void handleGroup(QTcpSocket *socket, const QJsonObject &obj);
    void handleCreateGroup(QTcpSocket *socket, const QJsonObject &obj);
    void handleJoinGroup(QTcpSocket *socket, const QJsonObject &obj);
    void handleLeaveGroup(QTcpSocket *socket, const QJsonObject &obj);

    void handleSearchUser(QTcpSocket *socket, const QJsonObject &obj);
    void handleFriendRequestSend(QTcpSocket *socket, const QJsonObject &obj);
    void handleFriendRequestReply(QTcpSocket *socket, const QJsonObject &obj);
    void handleAddFriend(QTcpSocket *socket, const QJsonObject &obj);
    void handleRemoveFriend(QTcpSocket *socket, const QJsonObject &obj);
    void handleListFriends(QTcpSocket *socket, const QJsonObject &obj);

    QString resolveSenderName(QTcpSocket *socket, const QJsonObject &obj) const;

    void sendJson(QTcpSocket *socket, const QJsonObject &obj);
    void sendAck(QTcpSocket *socket, const QString &requestId, bool ok, const QString &error = QString());
    void sendSystem(QTcpSocket *socket, const QString &text);
    void broadcastJson(const QJsonObject &obj);
    void broadcastSystem(const QString &text);
    void broadcastGroupSystem(const QString &group, const QString &text);
    void sendFriendList(QTcpSocket *socket, const QString &user);

    QTcpServer *m_server = nullptr;
    QHash<QTcpSocket *, ClientInfo> m_clients;
    QHash<QString, QTcpSocket *> m_userToSocket;
    QHash<QString, QString> m_userNicknames;
    QHash<QString, FriendRequest> m_friendRequests;
    quint64 m_friendRequestSeq = 1;
    GroupRegistry m_groups;
    FriendRegistry m_friends;
    AuthRegistry m_auth;
};
