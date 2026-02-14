#pragma once

#include <QWidget>

class QButtonGroup;

QT_BEGIN_NAMESPACE
namespace Ui {
class LeftBar;
}
QT_END_NAMESPACE

class LeftBar : public QWidget
{
    Q_OBJECT

public:
    explicit LeftBar(QWidget *parent = nullptr);
    ~LeftBar() override;
    void setCurrentUserName(const QString &userName);

signals:
    void navigationChanged(const QString &section);

private:
    Ui::LeftBar *ui = nullptr;
    QButtonGroup *m_navGroup = nullptr;
};
