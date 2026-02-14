#include "MainWindow.h"

#include "ChatClient.h"
#include "ui_MainWindow.h"

#include <QMessageBox>

/*
 * MainWindowActions.cpp（交互动作模块）
 * 只处理“用户发起的动作”：连接、发送、建群/加群/退群。
 */

void MainWindow::connectOrDisconnect()
{
    if (m_client->isConnected()) {
        m_client->disconnectFromServer();
        return;
    }

    bool ok = false;
    const int port = ui->portEdit->text().toInt(&ok);
    if (!ok || port <= 0 || port > 65535) {
        QMessageBox::warning(this, "端口错误", "请输入有效端口 (1-65535)。");
        return;
    }

    const QString user = ui->nameEdit->text().trimmed();
    if (user.isEmpty()) {
        QMessageBox::warning(this, "昵称错误", "昵称不能为空。");
        return;
    }

    m_client->setUserName(user);
    appendSystemMessage(QString("正在连接 %1:%2 ...").arg(ui->hostEdit->text(), QString::number(port)));
    m_client->connectToServer(ui->hostEdit->text().trimmed(), static_cast<quint16>(port));
}

void MainWindow::sendMessage()
{
    if (!m_client->isConnected()) {
        appendSystemMessage("尚未连接服务器。");
        return;
    }

    const QString text = ui->messageEdit->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    const QString mode = currentMode();
    const QString target = currentTarget();
    if (target.isEmpty()) {
        QMessageBox::warning(this, "目标不能为空", mode == "private" ? "请输入私聊对象用户名。" : "请输入群名。");
        return;
    }

    if (mode == "private") {
        if (!isFriend(target)) {
            QMessageBox::warning(this, "不是好友", "当前私聊对象不在好友列表，请先添加好友。");
            return;
        }
        // 私聊由当前服务端“仅转发给接收方”，发送方本地先写一条。
        m_client->sendPrivate(target, text);
        addConversationItem(mode, target);
        appendChatMessage("我", text, mode, target);
    } else {
        // 群聊由服务端回显给发送方，避免本地先写导致双份消息。
        m_client->sendGroup(target, text);
    }

    ui->messageEdit->clear();
}

void MainWindow::onSearchFriend()
{
    if (!m_client->isConnected()) {
        appendSystemMessage("尚未连接服务器。");
        return;
    }

    m_client->sendSearchUser(ui->friendSearchEdit->text().trimmed());
}

void MainWindow::onAddFriend()
{
    if (!m_client->isConnected()) {
        appendSystemMessage("尚未连接服务器。");
        return;
    }

    const QString name = ui->friendSearchEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "好友名不能为空", "请输入要添加的好友用户名。");
        return;
    }

    m_client->sendAddFriend(name);
}

void MainWindow::onRemoveFriend()
{
    if (!m_client->isConnected()) {
        appendSystemMessage("尚未连接服务器。");
        return;
    }

    const QString name = ui->friendSearchEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "好友名不能为空", "请输入要删除的好友用户名。");
        return;
    }

    m_client->sendRemoveFriend(name);
}

void MainWindow::onCreateGroup()
{
    if (!m_client->isConnected()) {
        appendSystemMessage("尚未连接服务器。");
        return;
    }

    const QString group = currentTarget();
    if (group.isEmpty()) {
        QMessageBox::warning(this, "群名不能为空", "请输入要创建的群名。");
        return;
    }

    // 注意：不在这里提前加左侧会话，等服务端确认成功再加。
    m_client->sendCreateGroup(group);
}

void MainWindow::onJoinGroup()
{
    if (!m_client->isConnected()) {
        appendSystemMessage("尚未连接服务器。");
        return;
    }

    const QString group = currentTarget();
    if (group.isEmpty()) {
        QMessageBox::warning(this, "群名不能为空", "请输入要加入的群名。");
        return;
    }

    // 注意：不在这里提前加左侧会话，等服务端确认成功再加。
    m_client->sendJoinGroup(group);
}

void MainWindow::onLeaveGroup()
{
    if (!m_client->isConnected()) {
        appendSystemMessage("尚未连接服务器。");
        return;
    }

    const QString group = currentTarget();
    if (group.isEmpty()) {
        QMessageBox::warning(this, "群名不能为空", "请输入要退出的群名。");
        return;
    }

    m_client->sendLeaveGroup(group);
}
