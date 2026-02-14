#include "SessionItemWidget.h"

#include "ui_SessionItemWidget.h"

#include <QFontMetrics>
#include <QResizeEvent>

SessionItemWidget::SessionItemWidget(QWidget *parent) : QWidget(parent), ui(new Ui::SessionItemWidget)
{
    ui->setupUi(this);

    setFixedHeight(72);
    ui->avatarLabel->setFixedSize(48, 48);

    QFont nameFont("Microsoft YaHei", 14);
    nameFont.setBold(true);
    ui->nameLabel->setFont(nameFont);

    QFont timeFont("Microsoft YaHei", 12);
    ui->timeLabel->setFont(timeFont);
    ui->summaryLabel->setFont(QFont("Microsoft YaHei", 12));
}

SessionItemWidget::~SessionItemWidget()
{
    delete ui;
}

void SessionItemWidget::setData(const QString &name, const QString &timeText, const QString &summaryText)
{
    m_name = name;
    m_timeText = timeText;
    m_summaryRaw = summaryText;

    ui->nameLabel->setText(m_name);
    ui->timeLabel->setText(m_timeText);

    const int maxWidth = ui->summaryLabel->width() > 0 ? ui->summaryLabel->width() : 170;
    const QFontMetrics fm(ui->summaryLabel->font());
    ui->summaryLabel->setText(fm.elidedText(m_summaryRaw, Qt::ElideRight, maxWidth));
}

void SessionItemWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    setData(m_name, m_timeText, m_summaryRaw);
}
