#pragma once

#include <QMainWindow>
#include <QMap>
#include <QPoint>

class LeftBar;
class SessionPanel;
class ChatPanel;
class ChatClient;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(ChatClient *client, QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    struct PrivateMessage {
        QString sender;
        QString text;
        bool outgoing = false;
    };

    Ui::MainWindow *ui = nullptr;
    LeftBar *m_leftBar = nullptr;
    SessionPanel *m_sessionPanel = nullptr;
    ChatPanel *m_chatPanel = nullptr;
    ChatClient *m_client = nullptr;
    QString m_currentPeer;
    QString m_currentSessionType = "direct";
    QString m_currentSessionId;
    QMap<QString, QList<PrivateMessage>> m_privateHistory;

    bool m_dragging = false;
    QPoint m_dragOffset;
};
