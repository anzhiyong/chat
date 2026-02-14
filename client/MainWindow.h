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
    Ui::MainWindow *ui = nullptr;
    LeftBar *m_leftBar = nullptr;
    SessionPanel *m_sessionPanel = nullptr;
    ChatPanel *m_chatPanel = nullptr;
    ChatClient *m_client = nullptr;
    QString m_currentPeer;
    QMap<QString, QStringList> m_privateHistory;

    bool m_dragging = false;
    QPoint m_dragOffset;
};
