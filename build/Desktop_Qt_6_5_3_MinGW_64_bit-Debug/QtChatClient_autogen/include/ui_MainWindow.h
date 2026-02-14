/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *outerLayout;
    QFrame *windowCard;
    QVBoxLayout *pageLayout;
    QFrame *titleBar;
    QHBoxLayout *titleLayout;
    QLabel *titleTextLabel;
    QSpacerItem *titleSpacer;
    QToolButton *minButton;
    QToolButton *maxButton;
    QToolButton *closeButton;
    QHBoxLayout *rootLayout;
    QWidget *leftContainer;
    QWidget *sessionContainer;
    QWidget *chatContainer;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1120, 700);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setStyleSheet(QString::fromUtf8("background: transparent;"));
        outerLayout = new QVBoxLayout(centralwidget);
        outerLayout->setSpacing(0);
        outerLayout->setObjectName("outerLayout");
        outerLayout->setContentsMargins(10, 10, 10, 10);
        windowCard = new QFrame(centralwidget);
        windowCard->setObjectName("windowCard");
        windowCard->setStyleSheet(QString::fromUtf8("QFrame#windowCard{background:#FFFFFF;border:1px solid #F0F0F0;border-radius:8px;}"));
        windowCard->setFrameShape(QFrame::NoFrame);
        pageLayout = new QVBoxLayout(windowCard);
        pageLayout->setSpacing(0);
        pageLayout->setObjectName("pageLayout");
        pageLayout->setContentsMargins(0, 0, 0, 0);
        titleBar = new QFrame(windowCard);
        titleBar->setObjectName("titleBar");
        titleBar->setMinimumSize(QSize(0, 36));
        titleBar->setMaximumSize(QSize(16777215, 36));
        titleBar->setStyleSheet(QString::fromUtf8("QFrame#titleBar{background:#FAFAFA;border:none;border-bottom:1px solid #F2F2F2;} QToolButton{border:none;background:transparent;color:#777777;min-width:34px;min-height:26px;} QToolButton:hover{background:#F0F0F0;} QToolButton#closeButton:hover{background:#E81123;color:white;} QLabel{color:#A0A0A0;}"));
        titleLayout = new QHBoxLayout(titleBar);
        titleLayout->setObjectName("titleLayout");
        titleLayout->setContentsMargins(10, 4, 4, 4);
        titleTextLabel = new QLabel(titleBar);
        titleTextLabel->setObjectName("titleTextLabel");

        titleLayout->addWidget(titleTextLabel);

        titleSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        titleLayout->addItem(titleSpacer);

        minButton = new QToolButton(titleBar);
        minButton->setObjectName("minButton");

        titleLayout->addWidget(minButton);

        maxButton = new QToolButton(titleBar);
        maxButton->setObjectName("maxButton");

        titleLayout->addWidget(maxButton);

        closeButton = new QToolButton(titleBar);
        closeButton->setObjectName("closeButton");

        titleLayout->addWidget(closeButton);


        pageLayout->addWidget(titleBar);

        rootLayout = new QHBoxLayout();
        rootLayout->setSpacing(0);
        rootLayout->setObjectName("rootLayout");
        rootLayout->setContentsMargins(0, 0, 0, 0);
        leftContainer = new QWidget(windowCard);
        leftContainer->setObjectName("leftContainer");
        leftContainer->setMinimumSize(QSize(70, 0));
        leftContainer->setMaximumSize(QSize(70, 16777215));

        rootLayout->addWidget(leftContainer);

        sessionContainer = new QWidget(windowCard);
        sessionContainer->setObjectName("sessionContainer");
        sessionContainer->setMinimumSize(QSize(280, 0));
        sessionContainer->setMaximumSize(QSize(300, 16777215));

        rootLayout->addWidget(sessionContainer);

        chatContainer = new QWidget(windowCard);
        chatContainer->setObjectName("chatContainer");

        rootLayout->addWidget(chatContainer);


        pageLayout->addLayout(rootLayout);


        outerLayout->addWidget(windowCard);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Qt Chat Client", nullptr));
        titleTextLabel->setText(QString());
        minButton->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        maxButton->setText(QCoreApplication::translate("MainWindow", "\342\226\241", nullptr));
        closeButton->setText(QCoreApplication::translate("MainWindow", "\303\227", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
