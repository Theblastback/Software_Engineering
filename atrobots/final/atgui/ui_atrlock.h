/********************************************************************************
** Form generated from reading UI file 'atrlock.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ATRLOCK_H
#define UI_ATRLOCK_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_atrlock
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLabel *rob_name;
    QProgressBar *progressBar;
    QHBoxLayout *horizontalLayout;
    QPushButton *selectRobot;
    QPushButton *lockRobot;

    void setupUi(QWidget *atrlock)
    {
        if (atrlock->objectName().isEmpty())
            atrlock->setObjectName(QStringLiteral("atrlock"));
        atrlock->resize(640, 480);
        QFont font;
        font.setFamily(QStringLiteral("ROG Fonts v1.6"));
        atrlock->setFont(font);
        verticalLayout = new QVBoxLayout(atrlock);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(atrlock);
        label->setObjectName(QStringLiteral("label"));
        QFont font1;
        font1.setPointSize(26);
        label->setFont(font1);
        label->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        verticalLayout->addWidget(label);

        rob_name = new QLabel(atrlock);
        rob_name->setObjectName(QStringLiteral("rob_name"));
        QFont font2;
        font2.setFamily(QStringLiteral("ROG Fonts v1.6"));
        font2.setPointSize(22);
        rob_name->setFont(font2);
        rob_name->setStyleSheet(QStringLiteral("color: red;"));
        rob_name->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(rob_name);

        progressBar = new QProgressBar(atrlock);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        QFont font3;
        font3.setPointSize(12);
        progressBar->setFont(font3);
        progressBar->setValue(0);

        verticalLayout->addWidget(progressBar);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        selectRobot = new QPushButton(atrlock);
        selectRobot->setObjectName(QStringLiteral("selectRobot"));
        QFont font4;
        font4.setPointSize(20);
        selectRobot->setFont(font4);

        horizontalLayout->addWidget(selectRobot);

        lockRobot = new QPushButton(atrlock);
        lockRobot->setObjectName(QStringLiteral("lockRobot"));
        lockRobot->setFont(font4);

        horizontalLayout->addWidget(lockRobot);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(atrlock);

        QMetaObject::connectSlotsByName(atrlock);
    } // setupUi

    void retranslateUi(QWidget *atrlock)
    {
        atrlock->setWindowTitle(QApplication::translate("atrlock", "ATRLock", nullptr));
        label->setText(QApplication::translate("atrlock", "Current Robot Selected", nullptr));
        rob_name->setText(QApplication::translate("atrlock", "None.", nullptr));
        selectRobot->setText(QApplication::translate("atrlock", "Select Robot", nullptr));
        lockRobot->setText(QApplication::translate("atrlock", "Lock Robot", nullptr));
    } // retranslateUi

};

namespace Ui {
    class atrlock: public Ui_atrlock {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ATRLOCK_H
