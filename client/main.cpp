#include <QApplication>
#include <QDialog>

#include "ChatClient.h"
#include "LoginDialog.h"
#include "MainWindow.h"

/*
 * main 函数 = 程序入口。
 * 你可以把它理解成“应用启动总开关”。
 */
int main(int argc, char *argv[])
{
    // QApplication 是所有 Qt Widgets 程序都需要的核心对象。
    // 它负责：事件循环、窗口管理、键盘鼠标消息分发等。
    QApplication app(argc, argv);

    // 先创建网络对象。后续主界面会复用这个连接。
    ChatClient client;

    // 先登录：连接成功才允许进入主界面。
    LoginDialog loginDialog(&client);
    if (loginDialog.exec() != QDialog::Accepted) {
        return 0;
    }

    // 登录成功后再创建主窗口对象（聊天界面）。
    MainWindow window(&client);

    // 登录完成后主动拉一次会话列表，中间栏显示服务端真实数据。
    client.sendSessionList(client.userName());
    client.sendListFriends();

    // 设置窗口初始大小（像素）。
    window.resize(760, 520);

    // 显示窗口。如果不 show()，窗口不会出现在屏幕上。
    window.show();

    // 启动事件循环。
    // 这行非常关键：程序会在这里等待/处理用户点击、网络回调等事件。
    return app.exec();
}
