#pragma once

#include <QDialog>

class ChatClient;

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginDialog;
}
QT_END_NAMESPACE

/*
 * LoginDialog = 程序登录入口窗口。
 *
 * 目标：
 * 1) 让用户输入服务器地址、端口、用户 ID
 * 2) 只有连接成功后才允许进入主界面
 */
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(ChatClient *client, QWidget *parent = nullptr);
    ~LoginDialog() override;

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onConnected();
    void onNetworkError(const QString &errorText);
    void onAuthLoginResult(bool ok, const QString &errorText, const QString &userId);
    void onAuthRegisterResult(bool ok, const QString &errorText);

private:
    Ui::LoginDialog *ui = nullptr;
    ChatClient *m_client = nullptr;
    bool m_waitingConnect = false;
    bool m_pendingLogin = false;
    bool m_pendingRegister = false;
    QString m_pendingAccount;
    QString m_pendingPassword;
};
