#include "MainWindow.h"

#include "ChatClient.h"
#include "ChatPanel.h"
#include "LeftBar.h"
#include "SessionPanel.h"
#include "ui_MainWindow.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>
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

    connect(m_sessionPanel, &SessionPanel::conversationSelected, this, [this](const QString &title) {
        m_currentPeer = title;
        m_chatPanel->showChatState(title);

        const QStringList lines = m_privateHistory.value(m_currentPeer);
        m_chatPanel->setMessageViewText(lines.join("\n\n"));
    });
    connect(m_sessionPanel, &SessionPanel::addFriendRequested, this, [this](const QString &account) {
        if (m_client) {
            m_client->sendFriendRequest(account, "你好，我想添加你为好友");
        }
    });

    // 登录后服务端返回真实会话列表时，刷新中间会话栏。
    if (m_client) {
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
                    if (mode != "private") {
                        return;
                    }

                    const QString me = m_client->userName();
                    const bool outgoingEcho = (sender == me);
                    const QString peer = outgoingEcho ? target : sender;
                    if (peer.isEmpty()) {
                        return;
                    }

                    const QString line = QString("%1: %2").arg(sender, text);
                    m_privateHistory[peer].push_back(line);

                    if (m_currentPeer == peer) {
                        m_chatPanel->appendMessageLine(line);
                    }
                });
    }

    connect(m_chatPanel, &ChatPanel::messageSendRequested, this, [this](const QString &text) {
        if (!m_client) {
            return;
        }
        if (m_currentPeer.isEmpty()) {
            QMessageBox::information(this, "提示", "请先在会话/好友列表中选择一个聊天对象。");
            return;
        }

        const QString me = m_client->userName();
        const QString line = QString("%1: %2").arg(me, text);
        m_privateHistory[m_currentPeer].push_back(line);
        m_chatPanel->appendMessageLine(line);
        m_client->sendPrivate(m_currentPeer, text);
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
