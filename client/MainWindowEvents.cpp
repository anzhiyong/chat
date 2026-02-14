#include "MainWindow.h"

#include "ChatClient.h"
#include "ui_MainWindow.h"

#include <QListWidgetItem>
#include <QSignalBlocker>

/*
 * MainWindowEvents.cpp（事件回调模块）
 * 只处理：
 * - UI 选择切换
 * - 网络层回调
 * - 页面可用状态切换
 */

void MainWindow::onConversationSelectionChanged()
{
    QListWidgetItem *item = ui->conversationList->currentItem();
    if (!item) {
        return;
    }

    const QString key = item->data(Qt::UserRole).toString();
    const QStringList parts = key.split(':');
    if (parts.size() != 2) {
        return;
    }

    {
        // 防止 setCurrentIndex/setText 触发联动回调造成重复刷新。
        const QSignalBlocker blocker1(ui->chatModeCombo);
        const QSignalBlocker blocker2(ui->targetEdit);
        ui->chatModeCombo->setCurrentIndex(parts[0] == "private" ? 0 : 1);
        ui->targetEdit->setText(parts[1]);
    }

    updateChatTitle();
    refreshChatView();
}

void MainWindow::onConnected()
{
    updateUiState(true);
    appendSystemMessage("连接成功。");

    // 登录并加入默认大厅群 all。
    m_client->sendJoin();
    m_client->sendJoinGroup("all");
    m_client->sendListFriends();
}

void MainWindow::onDisconnected()
{
    updateUiState(false);
    appendSystemMessage("连接已断开。");
}

void MainWindow::onNetworkError(const QString &text)
{
    appendSystemMessage(QString("网络错误: %1").arg(text));
}

void MainWindow::onSystemMessage(const QString &text)
{
    appendSystemMessage(text);

    // 只有服务端明确成功后，才将群会话加入左侧列表。
    const QString joinPrefix = QStringLiteral("已加入群 ");
    const QString createPrefix = QStringLiteral("创建群 ");

    if (text.startsWith(joinPrefix)) {
        const QString group = text.mid(joinPrefix.size()).trimmed();
        if (!group.isEmpty()) {
            addConversationItem("group", group);
        }
        return;
    }

    if (text.startsWith(createPrefix) && text.endsWith(QStringLiteral(" 成功"))) {
        QString group = text.mid(createPrefix.size());
        group.chop(QStringLiteral(" 成功").size());
        group = group.trimmed();
        if (!group.isEmpty()) {
            addConversationItem("group", group);
        }
    }

    if (text.startsWith(QStringLiteral("已添加 ")) && text.endsWith(QStringLiteral(" 为好友"))) {
        QString name = text.mid(QStringLiteral("已添加 ").size());
        name.chop(QStringLiteral(" 为好友").size());
        name = name.trimmed();
        if (!name.isEmpty()) {
            m_friends.insert(name);
            refreshFriendList();
            addConversationItem("private", name);
        }
        return;
    }

    if (text.startsWith(QStringLiteral("已删除好友 "))) {
        QString name = text.mid(QStringLiteral("已删除好友 ").size()).trimmed();
        if (!name.isEmpty()) {
            m_friends.remove(name);
            refreshFriendList();
        }
    }
}

void MainWindow::onChatMessage(const QString &sender, const QString &text, const QString &mode, const QString &target)
{
    if (mode == "private") {
        // 私聊会话以“对方用户名”为 key。
        const QString myName = ui->nameEdit->text().trimmed();
        const QString peer = (sender == myName) ? target : sender;

        addConversationItem("private", peer);
        appendChatMessage(sender, text, "private", peer);
        return;
    }

    addConversationItem("group", target);
    appendChatMessage(sender, text, "group", target);
}

void MainWindow::onFriendSearchResult(const QStringList &users)
{
    if (users.isEmpty()) {
        appendSystemMessage("搜索结果：未找到匹配用户。");
        return;
    }

    appendSystemMessage(QString("搜索结果：%1").arg(users.join(", ")));
}

void MainWindow::onFriendListUpdated(const QStringList &friends)
{
    m_friends = QSet<QString>(friends.begin(), friends.end());
    refreshFriendList();
}

void MainWindow::onFriendSelectionChanged()
{
    QListWidgetItem *item = ui->friendList->currentItem();
    if (!item) {
        return;
    }

    const QString name = item->text().trimmed();
    if (name.isEmpty()) {
        return;
    }

    addConversationItem("private", name);

    const QSignalBlocker blocker1(ui->chatModeCombo);
    const QSignalBlocker blocker2(ui->targetEdit);
    ui->chatModeCombo->setCurrentIndex(0);
    ui->targetEdit->setText(name);
    updateChatTitle();
}

void MainWindow::updateUiState(bool connected)
{
    ui->connectButton->setText(connected ? "断开" : "连接");
    ui->sendButton->setEnabled(connected);
    ui->messageEdit->setEnabled(connected);

    // 连接参数仅在未连接时允许编辑。
    ui->hostEdit->setEnabled(!connected);
    ui->portEdit->setEnabled(!connected);
    ui->nameEdit->setEnabled(!connected);

    ui->createGroupButton->setEnabled(connected);
    ui->joinGroupButton->setEnabled(connected);
    ui->leaveGroupButton->setEnabled(connected);
    ui->searchFriendButton->setEnabled(connected);
    ui->addFriendButton->setEnabled(connected);
    ui->removeFriendButton->setEnabled(connected);
    ui->friendSearchEdit->setEnabled(connected);
    ui->friendList->setEnabled(connected);
}
