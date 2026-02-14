/********************************************************************************
** Form generated from reading UI file 'LeftBar.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LEFTBAR_H
#define UI_LEFTBAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LeftBar
{
public:
    QVBoxLayout *rootLayout;
    QVBoxLayout *contentLayout;
    QLabel *avatarLabel;
    QSpacerItem *avatarGap;
    QVBoxLayout *navLayout;
    QToolButton *chatButton;
    QToolButton *contactButton;
    QToolButton *favButton;
    QToolButton *momentsButton;
    QToolButton *appsButton;
    QSpacerItem *stretchSpacer;
    QFrame *bottomSeparator;
    QSpacerItem *separatorGap;
    QToolButton *phoneButton;
    QSpacerItem *bottomButtonsGap;
    QToolButton *menuButton;

    void setupUi(QWidget *LeftBar)
    {
        if (LeftBar->objectName().isEmpty())
            LeftBar->setObjectName("LeftBar");
        rootLayout = new QVBoxLayout(LeftBar);
        rootLayout->setSpacing(0);
        rootLayout->setObjectName("rootLayout");
        rootLayout->setContentsMargins(0, 0, 0, 0);
        contentLayout = new QVBoxLayout();
        contentLayout->setSpacing(0);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(0, 20, 0, 14);
        avatarLabel = new QLabel(LeftBar);
        avatarLabel->setObjectName("avatarLabel");
        avatarLabel->setMinimumSize(QSize(40, 40));
        avatarLabel->setMaximumSize(QSize(40, 40));
        avatarLabel->setAlignment(Qt::AlignCenter);

        contentLayout->addWidget(avatarLabel);

        avatarGap = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        contentLayout->addItem(avatarGap);

        navLayout = new QVBoxLayout();
        navLayout->setSpacing(20);
        navLayout->setObjectName("navLayout");
        navLayout->setContentsMargins(0, 0, 0, 0);
        chatButton = new QToolButton(LeftBar);
        chatButton->setObjectName("chatButton");

        navLayout->addWidget(chatButton);

        contactButton = new QToolButton(LeftBar);
        contactButton->setObjectName("contactButton");

        navLayout->addWidget(contactButton);

        favButton = new QToolButton(LeftBar);
        favButton->setObjectName("favButton");

        navLayout->addWidget(favButton);

        momentsButton = new QToolButton(LeftBar);
        momentsButton->setObjectName("momentsButton");

        navLayout->addWidget(momentsButton);

        appsButton = new QToolButton(LeftBar);
        appsButton->setObjectName("appsButton");

        navLayout->addWidget(appsButton);


        contentLayout->addLayout(navLayout);

        stretchSpacer = new QSpacerItem(20, 80, QSizePolicy::Minimum, QSizePolicy::Expanding);

        contentLayout->addItem(stretchSpacer);

        bottomSeparator = new QFrame(LeftBar);
        bottomSeparator->setObjectName("bottomSeparator");
        bottomSeparator->setFrameShape(QFrame::NoFrame);

        contentLayout->addWidget(bottomSeparator);

        separatorGap = new QSpacerItem(20, 12, QSizePolicy::Minimum, QSizePolicy::Expanding);

        contentLayout->addItem(separatorGap);

        phoneButton = new QToolButton(LeftBar);
        phoneButton->setObjectName("phoneButton");

        contentLayout->addWidget(phoneButton);

        bottomButtonsGap = new QSpacerItem(20, 12, QSizePolicy::Minimum, QSizePolicy::Expanding);

        contentLayout->addItem(bottomButtonsGap);

        menuButton = new QToolButton(LeftBar);
        menuButton->setObjectName("menuButton");

        contentLayout->addWidget(menuButton);


        rootLayout->addLayout(contentLayout);


        retranslateUi(LeftBar);

        QMetaObject::connectSlotsByName(LeftBar);
    } // setupUi

    void retranslateUi(QWidget *LeftBar)
    {
        avatarLabel->setText(QString());
        chatButton->setText(QCoreApplication::translate("LeftBar", "\350\201\212", nullptr));
        contactButton->setText(QCoreApplication::translate("LeftBar", "\344\272\272", nullptr));
        favButton->setText(QCoreApplication::translate("LeftBar", "\350\227\217", nullptr));
        momentsButton->setText(QCoreApplication::translate("LeftBar", "\345\234\210", nullptr));
        appsButton->setText(QCoreApplication::translate("LeftBar", "\347\250\213", nullptr));
        phoneButton->setText(QCoreApplication::translate("LeftBar", "\346\234\272", nullptr));
        menuButton->setText(QCoreApplication::translate("LeftBar", "\342\211\241", nullptr));
        (void)LeftBar;
    } // retranslateUi

};

namespace Ui {
    class LeftBar: public Ui_LeftBar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LEFTBAR_H
