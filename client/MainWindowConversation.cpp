#include "MainWindow.h"

#include "ui_MainWindow.h"

#include <QDateTime>
#include <QListWidgetItem>
#include <QTextCursor>

/*
 * MainWindowConversation.cpp（会话与渲染模块）
 * 核心职责：
 * 1) 维护每个会话自己的消息历史
 * 2) 左侧切换时只渲染当前会话
 * 3) 切回会话可恢复历史（符合 IM 直觉）
 */

void MainWindow::appendSystemMessage(const QString &text)
{
    const QString stamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    const QString line = QString("[%1] [系统] %2").arg(stamp, text.toHtmlEscaped());

    // 系统消息默认写入当前会话，避免污染其它会话。
    appendLineToConversation(selectedConversationKey(), line);
}

void MainWindow::appendChatMessage(const QString &sender, const QString &text, const QString &mode, const QString &target)
{
    const QString stamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    const QString tag = mode == "private" ? QString("私聊->%1").arg(target) : QString("群:%1").arg(target);
    const QString line = QString("[%1] [%2] <%3> %4")
                             .arg(stamp, tag.toHtmlEscaped(), sender.toHtmlEscaped(), text.toHtmlEscaped());

    appendLineToConversation(conversationKey(mode, target), line);
}

void MainWindow::addConversationItem(const QString &mode, const QString &target)
{
    const QString key = conversationKey(mode, target);

    if (!m_conversationKeys.contains(key)) {
        m_conversationKeys.insert(key);

        const QString label = mode == "private" ? QString("[单聊] %1").arg(target)
                                                  : QString("[群聊] %1").arg(target);
        auto *item = new QListWidgetItem(label);
        item->setData(Qt::UserRole, key);
        ui->conversationList->addItem(item);
    }

    // 确保该会话在历史存储里已初始化。
    if (!m_conversationHistory.contains(key)) {
        m_conversationHistory.insert(key, {});
    }
}

void MainWindow::updateChatTitle()
{
    const QString mode = currentMode();
    const QString target = currentTarget();
    const QString displayTarget = target.isEmpty() ? "(未设置目标)" : target;

    ui->chatTitleLabel->setText(mode == "private" ? QString("正在私聊: %1").arg(displayTarget)
                                                    : QString("群聊: %1").arg(displayTarget));
}

QString MainWindow::currentMode() const
{
    return ui->chatModeCombo->currentData().toString();
}

QString MainWindow::currentTarget() const
{
    return ui->targetEdit->text().trimmed();
}

QString MainWindow::conversationKey(const QString &mode, const QString &target) const
{
    return QString("%1:%2").arg(mode, target);
}

QString MainWindow::selectedConversationKey() const
{
    QListWidgetItem *item = ui->conversationList->currentItem();
    if (!item) {
        return QStringLiteral("group:all");
    }
    return item->data(Qt::UserRole).toString();
}

void MainWindow::appendLineToConversation(const QString &key, const QString &line)
{
    m_conversationHistory[key].push_back(line);

    // 只有当前会话新增消息，才即时刷新右侧窗口。
    if (key == selectedConversationKey()) {
        refreshChatView();
    }
}

void MainWindow::refreshChatView()
{
    const QString key = selectedConversationKey();
    const QString content = m_conversationHistory.value(key).join('\n');

    ui->chatView->setPlainText(content);
    ui->chatView->moveCursor(QTextCursor::End);
}

void MainWindow::refreshFriendList()
{
    const QString previous = ui->friendList->currentItem() ? ui->friendList->currentItem()->text() : QString();

    ui->friendList->clear();
    QStringList names = m_friends.values();
    names.sort(Qt::CaseInsensitive);
    ui->friendList->addItems(names);

    if (!previous.isEmpty()) {
        const QList<QListWidgetItem *> found = ui->friendList->findItems(previous, Qt::MatchExactly);
        if (!found.isEmpty()) {
            ui->friendList->setCurrentItem(found.first());
        }
    }
}

bool MainWindow::isFriend(const QString &name) const
{
    return m_friends.contains(name.trimmed());
}
