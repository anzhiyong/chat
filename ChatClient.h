#pragma once

#include <QByteArray>
#include <QObject>

class QJsonObject;
class QTcpSocket;

/*
 * ChatClient = 客户端网络层。
 *
 * 职责：
 * 1) 管理 socket 连接状态
 * 2) 负责协议收发（NDJSON）
 * 3) 把网络事件转换成 UI 可消费的信号
 *
 * 设计原则：
 * - MainWindow 不直接依赖 QTcpSocket（降低耦合）
 * - 协议演进仅改 ChatClient/Server，UI 基本不受影响
 */
class ChatClient : public QObject
{
    Q_OBJECT

public:
    explicit ChatClient(QObject *parent = nullptr);

    void setUserName(const QString &name);
    QString userName() const;

    void connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();
    bool isConnected() const;

    // ===== 协议发送接口 =====
    void sendJoin();
    void sendPrivate(const QString &to, const QString &text);
    void sendGroup(const QString &group, const QString &text);
    void sendCreateGroup(const QString &group);
    void sendJoinGroup(const QString &group);
    void sendLeaveGroup(const QString &group);
    void sendSearchUser(const QString &keyword);
    void sendAddFriend(const QString &friendName);
    void sendRemoveFriend(const QString &friendName);
    void sendListFriends();

signals:
    // 网络状态。
    void connected();
    void disconnected();
    void networkError(const QString &errorText);

    // 业务事件（供 UI 订阅）。
    void systemMessage(const QString &text);
    void chatMessage(const QString &sender, const QString &text, const QString &mode, const QString &target);
    void friendSearchResult(const QStringList &users);
    void friendListUpdated(const QStringList &friends);

private slots:
    void onReadyRead();
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError();

private:
    void sendJson(const QJsonObject &obj);
    void processLine(const QByteArray &line);

    QTcpSocket *m_socket = nullptr;
    QString m_userName;

    // TCP 字节流缓冲；按 '\n' 切包。
    QByteArray m_receiveBuffer;
};
