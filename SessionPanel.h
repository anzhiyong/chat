#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class SessionPanel;
}
QT_END_NAMESPACE

class SessionPanel : public QWidget
{
    Q_OBJECT

public:
    explicit SessionPanel(QWidget *parent = nullptr);
    ~SessionPanel() override;

    void showChatPage();
    void showFriendPage();

signals:
    void conversationSelected(const QString &title);

private:
    void populateDemoSessions();
    void populateDemoFriends();

    Ui::SessionPanel *ui = nullptr;
};
