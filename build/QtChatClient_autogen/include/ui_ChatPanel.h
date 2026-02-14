/********************************************************************************
** Form generated from reading UI file 'ChatPanel.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHATPANEL_H
#define UI_CHATPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChatPanel
{
public:
    QVBoxLayout *rootLayout;
    QFrame *headerFrame;
    QHBoxLayout *headerLayout;
    QLabel *titleLabel;
    QSpacerItem *headerSpacer;
    QStackedWidget *stackedWidget;
    QWidget *emptyPage;
    QVBoxLayout *emptyLayout;
    QSpacerItem *emptyTopSpacer;
    QLabel *emptyIconLabel;
    QLabel *emptyHintLabel;
    QSpacerItem *emptyBottomSpacer;
    QWidget *chatPage;
    QVBoxLayout *chatPageLayout;
    QTextEdit *chatView;
    QFrame *inputFrame;
    QHBoxLayout *inputLayout;
    QLineEdit *messageEdit;
    QPushButton *sendButton;

    void setupUi(QWidget *ChatPanel)
    {
        if (ChatPanel->objectName().isEmpty())
            ChatPanel->setObjectName("ChatPanel");
        rootLayout = new QVBoxLayout(ChatPanel);
        rootLayout->setSpacing(0);
        rootLayout->setObjectName("rootLayout");
        rootLayout->setContentsMargins(0, 0, 0, 0);
        headerFrame = new QFrame(ChatPanel);
        headerFrame->setObjectName("headerFrame");
        headerFrame->setMinimumSize(QSize(0, 56));
        headerFrame->setMaximumSize(QSize(16777215, 56));
        headerLayout = new QHBoxLayout(headerFrame);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(16, 0, 16, 0);
        titleLabel = new QLabel(headerFrame);
        titleLabel->setObjectName("titleLabel");

        headerLayout->addWidget(titleLabel);

        headerSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        headerLayout->addItem(headerSpacer);


        rootLayout->addWidget(headerFrame);

        stackedWidget = new QStackedWidget(ChatPanel);
        stackedWidget->setObjectName("stackedWidget");
        emptyPage = new QWidget();
        emptyPage->setObjectName("emptyPage");
        emptyLayout = new QVBoxLayout(emptyPage);
        emptyLayout->setObjectName("emptyLayout");
        emptyLayout->setContentsMargins(0, 0, 0, 0);
        emptyTopSpacer = new QSpacerItem(20, 80, QSizePolicy::Minimum, QSizePolicy::Expanding);

        emptyLayout->addItem(emptyTopSpacer);

        emptyIconLabel = new QLabel(emptyPage);
        emptyIconLabel->setObjectName("emptyIconLabel");
        emptyIconLabel->setAlignment(Qt::AlignCenter);

        emptyLayout->addWidget(emptyIconLabel);

        emptyHintLabel = new QLabel(emptyPage);
        emptyHintLabel->setObjectName("emptyHintLabel");
        emptyHintLabel->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

        emptyLayout->addWidget(emptyHintLabel);

        emptyBottomSpacer = new QSpacerItem(20, 120, QSizePolicy::Minimum, QSizePolicy::Expanding);

        emptyLayout->addItem(emptyBottomSpacer);

        stackedWidget->addWidget(emptyPage);
        chatPage = new QWidget();
        chatPage->setObjectName("chatPage");
        chatPageLayout = new QVBoxLayout(chatPage);
        chatPageLayout->setSpacing(0);
        chatPageLayout->setObjectName("chatPageLayout");
        chatPageLayout->setContentsMargins(0, 0, 0, 0);
        chatView = new QTextEdit(chatPage);
        chatView->setObjectName("chatView");
        chatView->setReadOnly(true);

        chatPageLayout->addWidget(chatView);

        inputFrame = new QFrame(chatPage);
        inputFrame->setObjectName("inputFrame");
        inputFrame->setMinimumSize(QSize(0, 58));
        inputLayout = new QHBoxLayout(inputFrame);
        inputLayout->setSpacing(10);
        inputLayout->setObjectName("inputLayout");
        inputLayout->setContentsMargins(14, 10, 14, 10);
        messageEdit = new QLineEdit(inputFrame);
        messageEdit->setObjectName("messageEdit");

        inputLayout->addWidget(messageEdit);

        sendButton = new QPushButton(inputFrame);
        sendButton->setObjectName("sendButton");

        inputLayout->addWidget(sendButton);


        chatPageLayout->addWidget(inputFrame);

        stackedWidget->addWidget(chatPage);

        rootLayout->addWidget(stackedWidget);


        retranslateUi(ChatPanel);

        QMetaObject::connectSlotsByName(ChatPanel);
    } // setupUi

    void retranslateUi(QWidget *ChatPanel)
    {
        titleLabel->setText(QCoreApplication::translate("ChatPanel", "\345\276\256\344\277\241", nullptr));
        emptyIconLabel->setText(QString());
        emptyHintLabel->setText(QCoreApplication::translate("ChatPanel", "\346\234\252\351\200\211\346\213\251\350\201\212\345\244\251", nullptr));
        messageEdit->setPlaceholderText(QCoreApplication::translate("ChatPanel", "\350\276\223\345\205\245\346\266\210\346\201\257\357\274\214\346\214\211\345\233\236\350\275\246\345\217\221\351\200\201...", nullptr));
        sendButton->setText(QCoreApplication::translate("ChatPanel", "\345\217\221\351\200\201", nullptr));
        (void)ChatPanel;
    } // retranslateUi

};

namespace Ui {
    class ChatPanel: public Ui_ChatPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHATPANEL_H
