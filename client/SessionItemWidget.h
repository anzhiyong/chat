#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class SessionItemWidget;
}
QT_END_NAMESPACE

class SessionItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SessionItemWidget(QWidget *parent = nullptr);
    ~SessionItemWidget() override;

    void setData(const QString &name, const QString &timeText, const QString &summaryText);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::SessionItemWidget *ui = nullptr;
    QString m_name;
    QString m_timeText;
    QString m_summaryRaw;
};
