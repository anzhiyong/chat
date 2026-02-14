#include "LoginDialog.h"

#include "ChatClient.h"
#include "ui_LoginDialog.h"

LoginDialog::LoginDialog(ChatClient *client, QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog), m_client(client)
{
    ui->setupUi(this);

    setWindowTitle("登录");
    setModal(true);
    setFixedSize(420, 280);
    setFont(QFont("Microsoft YaHei", 10));

    ui->statusLabel->setText("请输入账号密码。系统会自动连接服务器并校验。");

    connect(ui->loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginDialog::onRegisterClicked);
    connect(m_client, &ChatClient::connected, this, &LoginDialog::onConnected);
    connect(m_client, &ChatClient::networkError, this, &LoginDialog::onNetworkError);
    connect(m_client, &ChatClient::authLoginResult, this, &LoginDialog::onAuthLoginResult);
    connect(m_client, &ChatClient::authRegisterResult, this, &LoginDialog::onAuthRegisterResult);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::onLoginClicked()
{
    if (m_waitingConnect) {
        return;
    }

    const QString account = ui->accountEdit->text().trimmed();
    const QString password = ui->passwordEdit->text();

    if (account.isEmpty() || password.isEmpty()) {
        ui->statusLabel->setText("账号和密码不能为空。");
        return;
    }

    m_pendingAccount = account;
    m_pendingPassword = password;
    m_pendingLogin = true;
    m_pendingRegister = false;

    ui->loginButton->setEnabled(false);
    ui->registerButton->setEnabled(false);

    if (m_client->isConnected()) {
        ui->statusLabel->setText("正在校验登录信息...");
        m_client->authLogin(m_pendingAccount, m_pendingPassword);
        return;
    }

    static const QString kServerHost = "127.0.0.1";
    static const quint16 kServerPort = 9000;
    m_waitingConnect = true;
    ui->statusLabel->setText(QString("正在连接服务器 %1:%2 ...").arg(kServerHost).arg(kServerPort));
    m_client->connectToServer(kServerHost, kServerPort);
}

void LoginDialog::onRegisterClicked()
{
    if (m_waitingConnect) {
        return;
    }

    const QString account = ui->accountEdit->text().trimmed();
    const QString password = ui->passwordEdit->text();

    if (account.isEmpty() || password.isEmpty()) {
        ui->statusLabel->setText("注册时账号和密码不能为空。");
        return;
    }

    m_pendingAccount = account;
    m_pendingPassword = password;
    m_pendingLogin = false;
    m_pendingRegister = true;

    ui->loginButton->setEnabled(false);
    ui->registerButton->setEnabled(false);

    if (m_client->isConnected()) {
        ui->statusLabel->setText("正在提交注册...");
        m_client->authRegister(m_pendingAccount, m_pendingPassword);
        return;
    }

    static const QString kServerHost = "127.0.0.1";
    static const quint16 kServerPort = 9000;
    m_waitingConnect = true;
    ui->statusLabel->setText(QString("正在连接服务器 %1:%2 ...").arg(kServerHost).arg(kServerPort));
    m_client->connectToServer(kServerHost, kServerPort);
}

void LoginDialog::onConnected()
{
    m_waitingConnect = false;
    if (m_pendingLogin) {
        ui->statusLabel->setText("正在校验登录信息...");
        m_client->authLogin(m_pendingAccount, m_pendingPassword);
        return;
    }
    if (m_pendingRegister) {
        ui->statusLabel->setText("正在提交注册...");
        m_client->authRegister(m_pendingAccount, m_pendingPassword);
    }
}

void LoginDialog::onNetworkError(const QString &errorText)
{
    m_waitingConnect = false;
    m_pendingLogin = false;
    m_pendingRegister = false;
    ui->loginButton->setEnabled(true);
    ui->registerButton->setEnabled(true);
    ui->statusLabel->setText(QString("连接失败：%1").arg(errorText));
}

void LoginDialog::onAuthLoginResult(bool ok, const QString &errorText, const QString &userId)
{
    ui->loginButton->setEnabled(true);
    ui->registerButton->setEnabled(true);
    m_pendingLogin = false;

    if (!ok) {
        ui->statusLabel->setText(QString("登录失败：%1").arg(errorText));
        return;
    }

    m_client->setUserName(userId);
    accept();
}

void LoginDialog::onAuthRegisterResult(bool ok, const QString &errorText)
{
    ui->loginButton->setEnabled(true);
    ui->registerButton->setEnabled(true);
    m_pendingRegister = false;

    if (ok) {
        ui->statusLabel->setText("注册成功，请点击“登录”。");
        return;
    }

    ui->statusLabel->setText(QString("注册失败：%1").arg(errorText));
}
