#pragma once

#include <QJsonArray>
#include <QStringList>
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
    void setSessionsFromServer(const QJsonArray &sessions);
    void setFriendsFromServer(const QStringList &friends);

signals:
    void conversationSelected(const QString &sessionType, const QString &sessionId, const QString &title);
    void addFriendRequested(const QString &account);
    void createGroupRequested(const QString &groupName);
    void joinGroupRequested(const QString &groupName);

private:
    void populateDemoSessions();
    void populateDemoFriends();
    void applySearchFilter(const QString &keyword);

    Ui::SessionPanel *ui = nullptr;
};
