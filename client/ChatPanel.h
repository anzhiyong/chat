#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChatPanel;
}
QT_END_NAMESPACE

class ChatPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPanel(QWidget *parent = nullptr);
    ~ChatPanel() override;

    void showEmptyState();
    void showChatState(const QString &title);

private:
    void setupStyles();
    void setupEmptyIcon();

    Ui::ChatPanel *ui = nullptr;
};
