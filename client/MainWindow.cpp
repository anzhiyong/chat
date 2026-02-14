#include "MainWindow.h"

#include "ChatPanel.h"
#include "LeftBar.h"
#include "SessionPanel.h"
#include "ui_MainWindow.h"

#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
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
        m_chatPanel->showChatState(title);
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
