/********************************************************************************
** Form generated from reading UI file 'idex.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IDEX_H
#define UI_IDEX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>
#include <file.h>
#include <friend.h>

QT_BEGIN_NAMESPACE

class Ui_Idex
{
public:
    QStackedWidget *stackedWidget;
    File *filePage;
    Friend *widget;
    QPushButton *fileBtn;
    QPushButton *friendBtn;

    void setupUi(QWidget *Idex)
    {
        if (Idex->objectName().isEmpty())
            Idex->setObjectName(QString::fromUtf8("Idex"));
        Idex->resize(387, 295);
        stackedWidget = new QStackedWidget(Idex);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        stackedWidget->setGeometry(QRect(60, 10, 301, 271));
        filePage = new File();
        filePage->setObjectName(QString::fromUtf8("filePage"));
        stackedWidget->addWidget(filePage);
        widget = new Friend();
        widget->setObjectName(QString::fromUtf8("widget"));
        stackedWidget->addWidget(widget);
        fileBtn = new QPushButton(Idex);
        fileBtn->setObjectName(QString::fromUtf8("fileBtn"));
        fileBtn->setGeometry(QRect(0, 0, 51, 131));
        QFont font;
        font.setFamily(QString::fromUtf8("Agency FB"));
        font.setPointSize(16);
        fileBtn->setFont(font);
        friendBtn = new QPushButton(Idex);
        friendBtn->setObjectName(QString::fromUtf8("friendBtn"));
        friendBtn->setGeometry(QRect(0, 140, 51, 141));
        friendBtn->setFont(font);

        retranslateUi(Idex);

        QMetaObject::connectSlotsByName(Idex);
    } // setupUi

    void retranslateUi(QWidget *Idex)
    {
        Idex->setWindowTitle(QCoreApplication::translate("Idex", "Form", nullptr));
        fileBtn->setText(QCoreApplication::translate("Idex", "\346\226\207\n"
"\344\273\266", nullptr));
        friendBtn->setText(QCoreApplication::translate("Idex", "\345\245\275\n"
"\345\217\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Idex: public Ui_Idex             �)��' ��"    ��      s e l e c t   u s e r _ i d  