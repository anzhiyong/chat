#include "ChatPanel.h"

#include "ui_ChatPanel.h"

#include <QColor>
#include <QFont>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>

ChatPanel::ChatPanel(QWidget *parent) : QWidget(parent), ui(new Ui::ChatPanel)
{
    ui->setupUi(this);

    setupStyles();
    setupEmptyIcon();
    showEmptyState();

    auto triggerSend = [this]() {
        const QString text = ui->messageEdit->text().trimmed();
        if (text.isEmpty()) {
            return;
        }
        ui->messageEdit->clear();
        emit messageSendRequested(text);
    };
    connect(ui->sendButton, &QPushButton::clicked, this, triggerSend);
    connect(ui->messageEdit, &QLineEdit::returnPressed, this, triggerSend);
}

ChatPanel::~ChatPanel()
{
    delete ui;
}

void ChatPanel::showEmptyState()
{
    ui->headerFrame->setVisible(false);
    ui->stackedWidget->setCurrentWidget(ui->emptyPage);
    ui->titleLabel->clear();
}

void ChatPanel::showChatState(const QString &title)
{
    ui->headerFrame->setVisible(true);
    ui->stackedWidget->setCurrentWidget(ui->chatPage);
    ui->titleLabel->setText(title);
}

void ChatPanel::setMessageViewText(const QString &text)
{
    ui->chatView->setPlainText(text);
}

void ChatPanel::appendMessageLine(const QString &line)
{
    if (!ui->chatView->toPlainText().isEmpty()) {
        ui->chatView->append("");
    }
    ui->chatView->append(line);
}

void ChatPanel::setupStyles()
{
    setStyleSheet(
        "QWidget#ChatPanel { background:#FFFFFF; }"
        "QFrame#headerFrame { background:#FFFFFF; border:none; border-bottom:1px solid #F0F0F0; }"
        "QLabel#titleLabel { color:#1F1F1F; font: 14px 'Microsoft YaHei'; }"
        "QLabel#emptyHintLabel { color:#B8B8B8; font: 11px 'Microsoft YaHei'; }"
        "QTextEdit#chatView { border:none; background:#FFFFFF; }"
        "QFrame#inputFrame { background:#FFFFFF; border:none; border-top:1px solid #F3F3F3; }"
        "QLineEdit#messageEdit { border:none; background:#FFFFFF; font: 13px 'Microsoft YaHei'; padding: 0 4px; }"
        "QPushButton#sendButton { border:none; border-radius:4px; background:#07C160; color:white; min-width:72px; min-height:30px; }"
        "QPushButton#sendButton:hover { background:#06AE57; }");
}

void ChatPanel::setupEmptyIcon()
{
    // 120px 的灰色气泡图标，透明度约 20%。
    QPixmap px(120, 120);
    px.fill(Qt::transparent);

    QPainter painter(&px);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(207, 207, 207, 51));

    QPainterPath bubble;
    bubble.addRoundedRect(QRectF(14, 22, 92, 70), 28, 28);

    QPainterPath tail;
    tail.moveTo(58, 86);
    tail.lineTo(48, 102);
    tail.lineTo(72, 88);
    tail.closeSubpath();

    painter.drawPath(bubble);
    painter.drawPath(tail);

    ui->emptyIconLabel->setPixmap(px);
    ui->emptyIconLabel->setFixedSize(120, 120);
}
