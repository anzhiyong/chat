#include "LeftBar.h"

#include "ui_LeftBar.h"

#include <QButtonGroup>
#include <QToolButton>

LeftBar::LeftBar(QWidget *parent) : QWidget(parent), ui(new Ui::LeftBar), m_navGroup(new QButtonGroup(this))
{
    ui->setupUi(this);

    setFixedWidth(70);
    m_navGroup->setExclusive(true);

    ui->contentLayout->setAlignment(ui->avatarLabel, Qt::AlignHCenter);

    auto initNavButton = [this](QToolButton *btn, bool checked = false) {
        btn->setCheckable(true);
        btn->setChecked(checked);
        btn->setAutoRaise(true);
        btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
        btn->setFixedSize(28, 28);
        m_navGroup->addButton(btn);
    };

    initNavButton(ui->chatButton, true);
    initNavButton(ui->contactButton);
    initNavButton(ui->favButton);
    initNavButton(ui->momentsButton);
    initNavButton(ui->appsButton);

    ui->navLayout->setAlignment(ui->chatButton, Qt::AlignHCenter);
    ui->navLayout->setAlignment(ui->contactButton, Qt::AlignHCenter);
    ui->navLayout->setAlignment(ui->favButton, Qt::AlignHCenter);
    ui->navLayout->setAlignment(ui->momentsButton, Qt::AlignHCenter);
    ui->navLayout->setAlignment(ui->appsButton, Qt::AlignHCenter);

    ui->phoneButton->setCheckable(false);
    ui->menuButton->setCheckable(false);
    ui->phoneButton->setAutoRaise(true);
    ui->menuButton->setAutoRaise(true);
    ui->phoneButton->setFixedSize(28, 28);
    ui->menuButton->setFixedSize(28, 28);
    ui->contentLayout->setAlignment(ui->phoneButton, Qt::AlignHCenter);
    ui->contentLayout->setAlignment(ui->menuButton, Qt::AlignHCenter);

    QFont font("Microsoft YaHei", 10);
    setFont(font);

    setStyleSheet(
        "QWidget#LeftBar { background-color: #2E2E2E; }"
        "QLabel#avatarLabel { background: #8F8F8F; border-radius: 20px; color:#FFFFFF; font: 600 10px 'Microsoft YaHei'; }"
        "QFrame#bottomSeparator { background: #3A3A3A; min-height: 1px; max-height: 1px; }"
        "QToolButton { color: #BFBFBF; border: none; border-radius: 6px; padding: 0; }"
        "QToolButton:hover { background: rgba(255,255,255,0.06); }"
        "QToolButton:checked { color: #07C160; background: rgba(7,193,96,0.12); }");

    connect(ui->chatButton, &QToolButton::clicked, this, [this]() { emit navigationChanged("chat"); });
    connect(ui->contactButton, &QToolButton::clicked, this, [this]() { emit navigationChanged("friend"); });
}

LeftBar::~LeftBar()
{
    delete ui;
}

void LeftBar::setCurrentUserName(const QString &userName)
{
    const QString name = userName.trimmed();
    if (name.isEmpty()) {
        ui->avatarLabel->clear();
        ui->avatarLabel->setToolTip(QString());
        return;
    }

    const QString shortText = name.size() > 2 ? name.left(2) : name;
    ui->avatarLabel->setText(shortText);
    ui->avatarLabel->setToolTip(name);
}
