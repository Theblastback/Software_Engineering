/********************************************************************************
** Form generated from reading UI file 'robotselection.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROBOTSELECTION_H
#define UI_ROBOTSELECTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RobotSelection
{
public:
    QHBoxLayout *horizontalLayout;
    QListWidget *listWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;

    void setupUi(QDialog *RobotSelection)
    {
        if (RobotSelection->objectName().isEmpty())
            RobotSelection->setObjectName(QStringLiteral("RobotSelection"));
        RobotSelection->resize(640, 480);
        horizontalLayout = new QHBoxLayout(RobotSelection);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        listWidget = new QListWidget(RobotSelection);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        horizontalLayout->addWidget(listWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        pushButton = new QPushButton(RobotSelection);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        QFont font;
        font.setFamily(QStringLiteral("ROG Fonts v1.6"));
        font.setPointSize(16);
        pushButton->setFont(font);

        verticalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(RobotSelection);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setFont(font);

        verticalLayout->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(RobotSelection);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setFont(font);

        verticalLayout->addWidget(pushButton_3);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(RobotSelection);

        QMetaObject::connectSlotsByName(RobotSelection);
    } // setupUi

    void retranslateUi(QDialog *RobotSelection)
    {
        RobotSelection->setWindowTitle(QApplication::translate("RobotSelection", "ATRobot Selection", nullptr));
#ifndef QT_NO_TOOLTIP
        pushButton->setToolTip(QApplication::translate("RobotSelection", "Select your ATRobot from the windows file explorer.", nullptr));
#endif // QT_NO_TOOLTIP
        pushButton->setText(QApplication::translate("RobotSelection", "Select ATRobots", nullptr));
#ifndef QT_NO_TOOLTIP
        pushButton_2->setToolTip(QApplication::translate("RobotSelection", "Select your ATRobot from the windows file explorer.", nullptr));
#endif // QT_NO_TOOLTIP
        pushButton_2->setText(QApplication::translate("RobotSelection", "Delete Selection", nullptr));
#ifndef QT_NO_TOOLTIP
        pushButton_3->setToolTip(QApplication::translate("RobotSelection", "Select your ATRobot from the windows file explorer.", nullptr));
#endif // QT_NO_TOOLTIP
        pushButton_3->setText(QApplication::translate("RobotSelection", "START GAME", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RobotSelection: public Ui_RobotSelection {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROBOTSELECTION_H
