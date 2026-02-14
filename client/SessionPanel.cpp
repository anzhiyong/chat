#include "SessionPanel.h"

#include "SessionItemWidget.h"
#include "ui_SessionPanel.h"

#include <QDateTime>
#include <QInputDialog>
#include <QJsonObject>
#include <QListWidgetItem>
#include <QMenu>

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
        emit conversationSelected(item->data(Qt::UserRole + 2).toString(), item->data(Qt::UserRole + 3).toString(),
                                 item->data(Qt::UserRole).toString());
    });

    connect(ui->friendList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (!item) {
            return;
        }
        emit conversationSelected("direct", item->text(), item->text());
    });

    // 新手提示：只要搜索框内容变化，就立即刷新当前页面列表显示。
    connect(ui->searchEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        applySearchFilter(text);
    });

    // 只做界面交互：+ 按钮弹出微信风格菜单（发起群聊 / 添加朋友）。
    connect(ui->addButton, &QToolButton::clicked, this, [this]() {
        QMenu menu(this);
        menu.setFont(QFont("Microsoft YaHei", 10));
        menu.setStyleSheet(
            "QMenu { background:#FFFFFF; border:1px solid #EDEDED; border-radius:8px; padding:6px 0; }"
            "QMenu::item { padding:8px 20px; color:#333333; }"
            "QMenu::item:selected { background:#F5F5F5; }");

        QAction *groupAction = menu.addAction("发起群聊");
        QAction *joinGroupAction = menu.addAction("加入群聊");
        QAction *addFriendAction = menu.addAction("添加朋友");

        const QPoint pos = ui->addButton->mapToGlobal(QPoint(ui->addButton->width() - 8, ui->addButton->height() + 6));
        QAction *selected = menu.exec(pos);
        if (selected == groupAction) {
            QInputDialog dialog(this);
            dialog.setWindowTitle("发起群聊");
            dialog.setLabelText("请输入群名称：");
            dialog.setTextEchoMode(QLineEdit::Normal);
            dialog.setStyleSheet(
                "QInputDialog { background:#FFFFFF; }"
                "QLabel { color:#222222; font: 12px 'Microsoft YaHei'; }"
                "QLineEdit { background:#FFFFFF; color:#222222; border:1px solid #DCDCDC; border-radius:4px; padding:4px; }"
                "QPushButton { min-width:72px; min-height:28px; border:1px solid #D0D0D0; border-radius:4px; background:#F7F7F7; color:#222222; }"
                "QPushButton:hover { background:#EFEFEF; }");

            const bool ok = (dialog.exec() == QDialog::Accepted);
            const QString groupName = dialog.textValue().trimmed();
            if (ok && !groupName.isEmpty()) {
                emit createGroupRequested(groupName);
            }
            return;
        }
        if (selected == joinGroupAction) {
            QInputDialog dialog(this);
            dialog.setWindowTitle("加入群聊");
            dialog.setLabelText("请输入群名称：");
            dialog.setTextEchoMode(QLineEdit::Normal);
            dialog.setStyleSheet(
                "QInputDialog { background:#FFFFFF; }"
                "QLabel { color:#222222; font: 12px 'Microsoft YaHei'; }"
                "QLineEdit { background:#FFFFFF; color:#222222; border:1px solid #DCDCDC; border-radius:4px; padding:4px; }"
                "QPushButton { min-width:72px; min-height:28px; border:1px solid #D0D0D0; border-radius:4px; background:#F7F7F7; color:#222222; }"
                "QPushButton:hover { background:#EFEFEF; }");

            const bool ok = (dialog.exec() == QDialog::Accepted);
            const QString groupName = dialog.textValue().trimmed();
            if (ok && !groupName.isEmpty()) {
                emit joinGroupRequested(groupName);
            }
            return;
        }
        if (selected == addFriendAction) {
            // 明确指定浅色样式，避免某些系统主题下出现黑底黑字不可读。
            QInputDialog dialog(this);
            dialog.setWindowTitle("添加朋友");
            dialog.setLabelText("请输入对方账号：");
            dialog.setTextEchoMode(QLineEdit::Normal);
            dialog.setStyleSheet(
                "QInputDialog { background:#FFFFFF; }"
                "QLabel { color:#222222; font: 12px 'Microsoft YaHei'; }"
                "QLineEdit { background:#FFFFFF; color:#222222; border:1px solid #DCDCDC; border-radius:4px; padding:4px; }"
                "QPushButton { min-width:72px; min-height:28px; border:1px solid #D0D0D0; border-radius:4px; background:#F7F7F7; color:#222222; }"
                "QPushButton:hover { background:#EFEFEF; }");

            bool ok = (dialog.exec() == QDialog::Accepted);
            const QString account = dialog.textValue();
            if (ok && !account.trimmed().isEmpty()) {
                emit addFriendRequested(account.trimmed());
            }
        }
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
    applySearchFilter(ui->searchEdit->text());
}

void SessionPanel::showFriendPage()
{
    ui->searchEdit->setPlaceholderText("搜索好友");
    ui->stackedWidget->setCurrentWidget(ui->friendPage);
    applySearchFilter(ui->searchEdit->text());
}

void SessionPanel::setSessionsFromServer(const QJsonArray &sessions)
{
    // 用服务端返回结果覆盖本地演示会话，避免 UI 一直显示假数据。
    ui->sessionList->clear();

    for (const QJsonValue &value : sessions) {
        if (!value.isObject()) {
            continue;
        }
        const QJsonObject obj = value.toObject();

        const QString title = obj.value("title").toString();
        const QString summary = obj.value("last_text").toString();
        const QString sessionType = obj.value("session_type").toString("direct");
        const QString sessionId = obj.value("session_id").toString(title);
        const qint64 ts = static_cast<qint64>(obj.value("last_ts").toDouble(0));
        const QString timeText = ts > 0 ? QDateTime::fromSecsSinceEpoch(ts).toString("MM-dd hh:mm") : QString();

        auto *item = new QListWidgetItem();
        item->setSizeHint(QSize(0, 72));
        item->setData(Qt::UserRole, title);
        item->setData(Qt::UserRole + 1, summary);
        item->setData(Qt::UserRole + 2, sessionType);
        item->setData(Qt::UserRole + 3, sessionId);

        auto *widget = new SessionItemWidget(ui->sessionList);
        widget->setData(title, timeText, summary);

        ui->sessionList->addItem(item);
        ui->sessionList->setItemWidget(item, widget);
    }

    if (ui->sessionList->count() > 0) {
        ui->sessionList->setCurrentRow(0);
    }

    applySearchFilter(ui->searchEdit->text());
}

void SessionPanel::setFriendsFromServer(const QStringList &friends)
{
    // 用服务端好友列表覆盖默认演示好友，确保数据一致。
    ui->friendList->clear();
    ui->friendList->addItems(friends);

    if (ui->friendList->count() > 0) {
        ui->friendList->setCurrentRow(0);
    }

    applySearchFilter(ui->searchEdit->text());
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
        item->setData(Qt::UserRole + 1, d.summary);
        item->setData(Qt::UserRole + 2, "direct");
        item->setData(Qt::UserRole + 3, d.name);

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

void SessionPanel::applySearchFilter(const QString &keyword)
{
    // 去掉首尾空格，避免用户误输入空格导致看起来“搜不到”。
    const QString trimmed = keyword.trimmed();
    const bool emptyKeyword = trimmed.isEmpty();

    // 聊天页：按会话名 + 最近消息内容做过滤。
    if (ui->stackedWidget->currentWidget() == ui->chatPage) {
        for (int i = 0; i < ui->sessionList->count(); ++i) {
            QListWidgetItem *item = ui->sessionList->item(i);
            QString candidate = item->data(Qt::UserRole).toString();
            candidate += " " + item->data(Qt::UserRole + 1).toString();

            const bool matched = emptyKeyword || candidate.contains(trimmed, Qt::CaseInsensitive);
            item->setHidden(!matched);
        }
        return;
    }

    // 好友页：按好友名字过滤。
    for (int i = 0; i < ui->friendList->count(); ++i) {
        QListWidgetItem *item = ui->friendList->item(i);
        const bool matched = emptyKeyword || item->text().contains(trimmed, Qt::CaseInsensitive);
        item->setHidden(!matched);
    }
}
