#include "MainWindow.h"

#include "ChatClient.h"
#include "ChatPanel.h"
#include "LeftBar.h"
#include "SessionPanel.h"
#include "ui_MainWindow.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QMouseEvent>
#include <QVBoxLayout>

MainWindow::MainWindow(ChatClient *client, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_client(client)
{
    ui->setupUi(this);

    // 高仿窗口：无边框 + 自定义标题栏。
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    // 为窗口边框增加柔和阴影。
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(28);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0, 0, 0, 55));
    ui->windowCard->setGraphicsEffect(shadow);

    // 统一字体。
    setFont(QFont("Microsoft YaHei", 10));

    // 整体分层：极淡分割线，低对比度，符合微信风格。
    setStyleSheet(
        "QWidget#sessionContainer { border-right:1px solid #F1F1F1; }"
        "QWidget#chatContainer { background:#FFFFFF; }");

    m_leftBar = new LeftBar(this);
    m_sessionPanel = new SessionPanel(this);
    m_chatPanel = new ChatPanel(this);

    auto *leftLayout = new QVBoxLayout(ui->leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(m_leftBar);

    auto *sessionLayout = new QVBoxLayout(ui->sessionContainer);
    sessionLayout->setContentsMargins(0, 0, 0, 0);
    sessionLayout->addWidget(m_sessionPanel);

    auto *chatLayout = new QVBoxLayout(ui->chatContainer);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    chatLayout->addWidget(m_chatPanel);

    connect(m_leftBar, &LeftBar::navigationChanged, this, [this](const QString &section) {
        if (section == "friend") {
            m_sessionPanel->showFriendPage();
            m_chatPanel->showEmptyState();
        } else if (section == "chat") {
            m_sessionPanel->showChatPage();
        }
    });

    connect(m_sessionPanel, &SessionPanel::conversationSelected, this,
            [this](const QString &sessionType, const QString &sessionId, const QString &title) {
        m_currentSessionType = sessionType;
        m_currentSessionId = sessionId;
        m_currentPeer = title;
        m_chatPanel->showChatState(title);
        m_chatPanel->clearMessages();
        const QString historyKey = (sessionType == "group") ? QString("group:%1").arg(sessionId) : sessionId;
        const QList<PrivateMessage> history = m_privateHistory.value(historyKey);
        for (const PrivateMessage &msg : history) {
            m_chatPanel->appendMessage(msg.sender, msg.text, msg.outgoing);
        }
        if (m_client && sessionType == "direct") {
            m_client->sendHistoryPull(m_currentSessionId, 100);
        }
    });
    connect(m_sessionPanel, &SessionPanel::createGroupRequested, this, [this](const QString &groupName) {
        if (!m_client) {
            return;
        }
        m_client->sendCreateGroup(groupName);
        // 创建后刷新会话列表，尽快把新群显示出来。
        m_client->sendSessionList(m_client->userName());
    });
    connect(m_sessionPanel, &SessionPanel::joinGroupRequested, this, [this](const QString &groupName) {
        if (!m_client) {
            return;
        }
        m_client->sendJoinGroup(groupName);
        // 加群后刷新会话列表，B 才能看到该群会话。
        m_client->sendSessionList(m_client->userName());
    });
    connect(m_sessionPanel, &SessionPanel::addFriendRequested, this, [this](const QString &account) {
        if (m_client) {
            m_client->sendFriendRequest(account, "你好，我想添加你为好友");
        }
    });

    // 登录后服务端返回真实会话列表时，刷新中间会话栏。
    if (m_client) {
        m_leftBar->setCurrentUserName(m_client->userName());
        connect(m_client, &ChatClient::sessionListReceived, m_sessionPanel, &SessionPanel::setSessionsFromServer);
        connect(m_client, &ChatClient::friendListUpdated, m_sessionPanel, &SessionPanel::setFriendsFromServer);
        connect(m_client, &ChatClient::friendRequestReceived, this,
                [this](const QString &requestId, const QString &from, const QString &remark) {
                    const QString text =
                        QString("%1 向你发送了好友申请。\n备注：%2\n是否同意？").arg(from, remark.isEmpty() ? "（无）" : remark);
                    const auto ans = QMessageBox::question(this, "好友申请", text, QMessageBox::Yes | QMessageBox::No,
                                                           QMessageBox::Yes);
                    m_client->sendFriendRequestReply(requestId, ans == QMessageBox::Yes);
                });
        connect(m_client, &ChatClient::friendRequestResult, this,
                [this](const QString &, bool accepted, const QString &, const QString &to) {
                    QMessageBox::information(this, "好友申请结果",
                                             accepted ? QString("%1 已通过你的好友申请").arg(to)
                                                      : QString("%1 拒绝了你的好友申请").arg(to));
                });
        connect(m_client, &ChatClient::chatMessage, this,
                [this](const QString &sender, const QString &text, const QString &mode, const QString &target) {
                    const QString me = m_client->userName();
                    QString historyKey;
                    if (mode == "private") {
                        const bool outgoingEcho = (sender == me);
                        const QString peer = outgoingEcho ? target : sender;
                        if (peer.isEmpty()) {
                            return;
                        }
                        historyKey = peer;
                    } else if (mode == "group") {
                        if (target.isEmpty()) {
                            return;
                        }
                        historyKey = QString("group:%1").arg(target);
                    } else {
                        return;
                    }

                    PrivateMessage msg;
                    msg.sender = sender;
                    msg.text = text;
                    msg.outgoing = (sender == me);
                    m_privateHistory[historyKey].push_back(msg);

                    const bool isCurrentGroup = (m_currentSessionType == "group" &&
                                                 historyKey == QString("group:%1").arg(m_currentSessionId));
                    const bool isCurrentDirect = (m_currentSessionType != "group" && historyKey == m_currentSessionId);
                    if (isCurrentGroup || isCurrentDirect) {
                        m_chatPanel->appendMessage(msg.sender, msg.text, msg.outgoing);
                    }
                });
        connect(m_client, &ChatClient::historyReceived, this, [this](const QString &peerId, const QJsonArray &history) {
            QList<PrivateMessage> parsed;
            const QString me = m_client->userName();
            for (const QJsonValue &v : history) {
                if (!v.isObject()) {
                    continue;
                }
                const QJsonObject obj = v.toObject();
                PrivateMessage msg;
                msg.sender = obj.value("from").toString();
                msg.text = obj.value("text").toString();
                msg.outgoing = (msg.sender == me);
                parsed.push_back(msg);
            }
            m_privateHistory[peerId] = parsed;

            if (m_currentPeer == peerId) {
                m_chatPanel->clearMessages();
                for (const PrivateMessage &msg : parsed) {
                    m_chatPanel->appendMessage(msg.sender, msg.text, msg.outgoing);
                }
            }
        });
    }

    connect(m_chatPanel, &ChatPanel::messageSendRequested, this, [this](const QString &text) {
        if (!m_client) {
            return;
        }
        if (m_currentSessionId.isEmpty()) {
            QMessageBox::information(this, "提示", "请先在会话/好友列表中选择一个聊天对象。");
            return;
        }

        const QString me = m_client->userName();
        if (m_currentSessionType == "group") {
            // 群聊由服务端统一广播回显，避免“本地先加一条 + 回显再加一条”导致重复。
            m_client->sendGroup(m_currentSessionId, text);
        } else {
            PrivateMessage msg;
            msg.sender = me;
            msg.text = text;
            msg.outgoing = true;
            const QString historyKey = m_currentSessionId;
            m_privateHistory[historyKey].push_back(msg);
            m_chatPanel->appendMessage(msg.sender, msg.text, msg.outgoing);
            m_client->sendPrivate(m_currentSessionId, text);
        }
    });

    connect(ui->minButton, &QToolButton::clicked, this, &MainWindow::showMinimized);
    connect(ui->maxButton, &QToolButton::clicked, this, [this]() {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });
    connect(ui->closeButton, &QToolButton::clicked, this, &MainWindow::close);

    ui->titleBar->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->titleBar) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto *e = static_cast<QMouseEvent *>(event);
            if (e->button() == Qt::LeftButton) {
                m_dragging = true;
                m_dragOffset = e->globalPosition().toPoint() - frameGeometry().topLeft();
                return true;
            }
        }

        if (event->type() == QEvent::MouseMove && m_dragging) {
            auto *e = static_cast<QMouseEvent *>(event);
            move(e->globalPosition().toPoint() - m_dragOffset);
            return true;
        }

        if (event->type() == QEvent::MouseButtonRelease) {
            auto *e = static_cast<QMouseEvent *>(event);
            if (e->button() == Qt::LeftButton) {
                m_dragging = false;
                return true;
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}
