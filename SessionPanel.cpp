#include "SessionPanel.h"

#include "SessionItemWidget.h"
#include "ui_SessionPanel.h"

#include <QListWidgetItem>

SessionPanel::SessionPanel(QWidget *parent) : QWidget(parent), ui(new Ui::SessionPanel)
{
    ui->setupUi(this);

    setFixedWidth(280);
    setFont(QFont("Microsoft YaHei", 10));

    setStyleSheet(
        "QWidget#SessionPanel { background:#F5F5F5; }"
        "QFrame#searchFrame { background:#F5F5F5; border:none; border-bottom:1px solid #F0F0F0; }"
        "QLineEdit#searchEdit { background:#EDEDED; border:none; border-radius:6px; padding:0 10px; min-height:32px; }"
        "QToolButton#addButton { border:none; background:transparent; color:#707070; min-width:28px; min-height:28px; }"
        "QToolButton#addButton:hover { background:rgba(0,0,0,0.04); border-radius:6px; }"
        "QListWidget#sessionList, QListWidget#friendList { background:#F5F5F5; border:none; outline:none; }"
        "QListWidget#sessionList::item, QListWidget#friendList::item { background:#F5F5F5; border:none; }"
        "QListWidget#sessionList::item:selected, QListWidget#friendList::item:selected { background:#D9D9D9; }"
        "QListWidget#sessionList::item:hover, QListWidget#friendList::item:hover { background:#ECECEC; }"
        "QScrollBar:vertical { width:8px; background:transparent; }"
        "QScrollBar::handle:vertical { background:rgba(0,0,0,0.10); border-radius:4px; min-height:24px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0px; }");

    ui->sessionList->setSpacing(0);
    ui->sessionList->setUniformItemSizes(true);

    populateDemoSessions();
    populateDemoFriends();
    showChatPage();

    connect(ui->sessionList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (!item) {
            return;
        }
        emit conversationSelected(item->data(Qt::UserRole).toString());
    });

    connect(ui->friendList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (!item) {
            return;
        }
        emit conversationSelected(item->text());
    });
}

SessionPanel::~SessionPanel()
{
    delete ui;
}

void SessionPanel::showChatPage()
{
    ui->searchEdit->setPlaceholderText("搜索会话");
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
}

void SessionPanel::showFriendPage()
{
    ui->searchEdit->setPlaceholderText("搜索好友");
    ui->stackedWidget->setCurrentWidget(ui->friendPage);
}

void SessionPanel::populateDemoSessions()
{
    struct DemoItem {
        QString name;
        QString time;
        QString summary;
    };

    const QList<DemoItem> demos = {
        {"文件传输助手", "12:42", "你可以在这里发送文件与消息"},
        {"产品群", "11:38", "下周评审时间改到周三下午三点"},
        {"张三", "昨天", "收到，晚上再确认一下细节"},
        {"设计讨论组", "周一", "视觉稿已更新到 v3，请查看"},
    };

    for (const DemoItem &d : demos) {
        auto *item = new QListWidgetItem();
        item->setSizeHint(QSize(0, 72));
        item->setData(Qt::UserRole, d.name);

        auto *widget = new SessionItemWidget(ui->sessionList);
        widget->setData(d.name, d.time, d.summary);

        ui->sessionList->addItem(item);
        ui->sessionList->setItemWidget(item, widget);
    }

    if (ui->sessionList->count() > 0) {
        ui->sessionList->setCurrentRow(0);
    }
}

void SessionPanel::populateDemoFriends()
{
    ui->friendList->addItems({"张三", "李四", "产品经理", "测试同学"});
    if (ui->friendList->count() > 0) {
        ui->friendList->setCurrentRow(0);
    }
}
