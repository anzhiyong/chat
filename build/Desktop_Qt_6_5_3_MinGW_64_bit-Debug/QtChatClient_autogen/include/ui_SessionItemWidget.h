/********************************************************************************
** Form generated from reading UI file 'SessionItemWidget.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SESSIONITEMWIDGET_H
#define UI_SESSIONITEMWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SessionItemWidget
{
public:
    QHBoxLayout *rootLayout;
    QLabel *avatarLabel;
    QVBoxLayout *textLayout;
    QHBoxLayout *headRow;
    QLabel *nameLabel;
    QSpacerItem *headSpacer;
    QLabel *timeLabel;
    QLabel *summaryLabel;

    void setupUi(QWidget *SessionItemWidget)
    {
        if (SessionItemWidget->objectName().isEmpty())
            SessionItemWidget->setObjectName("SessionItemWidget");
        SessionItemWidget->setMinimumSize(QSize(0, 72));
        SessionItemWidget->setMaximumSize(QSize(16777215, 72));
        rootLayout = new QHBoxLayout(SessionItemWidget);
        rootLayout->setSpacing(10);
        rootLayout->setObjectName("rootLayout");
        rootLayout->setContentsMargins(12, 12, 10, 12);
        avatarLabel = new QLabel(SessionItemWidget);
        avatarLabel->setObjectName("avatarLabel");
        avatarLabel->setMinimumSize(QSize(48, 48));
        avatarLabel->setMaximumSize(QSize(48, 48));
        avatarLabel->setStyleSheet(QString::fromUtf8("background:#CFCFCF; border-radius:8px;"));

        rootLayout->addWidget(avatarLabel);

        textLayout = new QVBoxLayout();
        textLayout->setSpacing(4);
        textLayout->setObjectName("textLayout");
        textLayout->setContentsMargins(0, 0, 0, 0);
        headRow = new QHBoxLayout();
        headRow->setSpacing(6);
        headRow->setObjectName("headRow");
        nameLabel = new QLabel(SessionItemWidget);
        nameLabel->setObjectName("nameLabel");

        headRow->addWidget(nameLabel);

        headSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        headRow->addItem(headSpacer);

        timeLabel = new QLabel(SessionItemWidget);
        timeLabel->setObjectName("timeLabel");
        timeLabel->setStyleSheet(QString::fromUtf8("color:#999999;"));

        headRow->addWidget(timeLabel);


        textLayout->addLayout(headRow);

        summaryLabel = new QLabel(SessionItemWidget);
        summaryLabel->setObjectName("summaryLabel");
        summaryLabel->setStyleSheet(QString::fromUtf8("color:#666666;"));

        textLayout->addWidget(summaryLabel);


        rootLayout->addLayout(textLayout);


        retranslateUi(SessionItemWidget);

        QMetaObject::connectSlotsByName(SessionItemWidget);
    } // setupUi

    void retranslateUi(QWidget *SessionItemWidget)
    {
        avatarLabel->setText(QString());
        nameLabel->setText(QCoreApplication::translate("SessionItemWidget", "\344\274\232\350\257\235\345\220\215\347\247\260", nullptr));
        timeLabel->setText(QCoreApplication::translate("SessionItemWidget", "12:00", nullptr));
        summaryLabel->setText(QCoreApplication::translate("SessionItemWidget", "\346\234\200\345\220\216\344\270\200\346\235\241\346\266\210\346\201\257\345\206\205\345\256\271\351\242\204\350\247\210", nullptr));
        (void)SessionItemWidget;
    } // retranslateUi

};

namespace Ui {
    class SessionItemWidget: public Ui_SessionItemWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SESSIONITEMWIDGET_H
