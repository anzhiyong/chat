#pragma once

#include <QMainWindow>
#include <QPoint>

class LeftBar;
class SessionPanel;
class ChatPanel;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::MainWindow *ui = nullptr;
    LeftBar *m_leftBar = nullptr;
    SessionPanel *m_sessionPanel = nullptr;
    ChatPanel *m_chatPanel = nullptr;

    bool m_dragging = false;
    QPoint m_dragOffset;
};
