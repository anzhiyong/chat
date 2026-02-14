/********************************************************************************
** Form generated from reading UI file 'SessionPanel.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SESSIONPANEL_H
#define UI_SESSIONPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SessionPanel
{
public:
    QVBoxLayout *rootLayout;
    QFrame *searchFrame;
    QHBoxLayout *searchRow;
    QLineEdit *searchEdit;
    QToolButton *addButton;
    QStackedWidget *stackedWidget;
    QWidget *chatPage;
    QVBoxLayout *chatPageLayout;
    QListWidget *sessionList;
    QWidget *friendPage;
    QVBoxLayout *friendPageLayout;
    QListWidget *friendList;

    void setupUi(QWidget *SessionPanel)
    {
        if (SessionPanel->objectName().isEmpty())
            SessionPanel->setObjectName("SessionPanel");
        rootLayout = new QVBoxLayout(SessionPanel);
        rootLayout->setSpacing(0);
        rootLayout->setObjectName("rootLayout");
        rootLayout->setContentsMargins(0, 0, 0, 0);
        searchFrame = new QFrame(SessionPanel);
        searchFrame->setObjectName("searchFrame");
        searchFrame->setMinimumSize(QSize(0, 50));
        searchFrame->setMaximumSize(QSize(16777215, 50));
        searchRow = new QHBoxLayout(searchFrame);
        searchRow->setSpacing(8);
        searchRow->setObjectName("searchRow");
        searchRow->setContentsMargins(10, 9, 10, 9);
        searchEdit = new QLineEdit(searchFrame);
        searchEdit->setObjectName("searchEdit");

        searchRow->addWidget(searchEdit);

        addButton = new QToolButton(searchFrame);
        addButton->setObjectName("addButton");

        searchRow->addWidget(addButton);


        rootLayout->addWidget(searchFrame);

        stackedWidget = new QStackedWidget(SessionPanel);
        stackedWidget->setObjectName("stackedWidget");
        chatPage = new QWidget();
        chatPage->setObjectName("chatPage");
        chatPageLayout = new QVBoxLayout(chatPage);
        chatPageLayout->setSpacing(0);
        chatPageLayout->setObjectName("chatPageLayout");
        chatPageLayout->setContentsMargins(0, 0, 0, 0);
        sessionList = new QListWidget(chatPage);
        sessionList->setObjectName("sessionList");

        chatPageLayout->addWidget(sessionList);

        stackedWidget->addWidget(chatPage);
        friendPage = new QWidget();
        friendPage->setObjectName("friendPage");
        friendPageLayout = new QVBoxLayout(friendPage);
        friendPageLayout->setSpacing(0);
        friendPageLayout->setObjectName("friendPageLayout");
        friendPageLayout->setContentsMargins(0, 0, 0, 0);
        friendList = new QListWidget(friendPage);
        friendList->setObjectName("friendList");

        friendPageLayout->addWidget(friendList);

        stackedWidget->addWidget(friendPage);

        rootLayout->addWidget(stackedWidget);


        retranslateUi(SessionPanel);

        QMetaObject::connectSlotsByName(SessionPanel);
    } // setupUi

    void retranslateUi(QWidget *SessionPanel)
    {
        searchEdit->setPlaceholderText(QCoreApplication::translate("SessionPanel", "\346\220\234\347\264\242\344\274\232\350\257\235", nullptr));
        addButton->setText(QCoreApplication::translate("SessionPanel", "+", nullptr));
        (void)SessionPanel;
    } // retranslateUi

};

namespace Ui {
    class SessionPanel: public Ui_SessionPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SESSIONPANEL_H
