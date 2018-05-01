/********************************************************************************
** Form generated from reading UI file 'roboteditor.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROBOTEDITOR_H
#define UI_ROBOTEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RobotEditor
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *plainTextEdit;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_2;
    QPushButton *pushButton;

    void setupUi(QDialog *RobotEditor)
    {
        if (RobotEditor->objectName().isEmpty())
            RobotEditor->setObjectName(QStringLiteral("RobotEditor"));
        RobotEditor->resize(640, 480);
        verticalLayout_2 = new QVBoxLayout(RobotEditor);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        plainTextEdit = new QPlainTextEdit(RobotEditor);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));

        verticalLayout->addWidget(plainTextEdit);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton_2 = new QPushButton(RobotEditor);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        QFont font;
        font.setFamily(QStringLiteral("ROG Fonts v1.6"));
        font.setPointSize(16);
        pushButton_2->setFont(font);

        horizontalLayout->addWidget(pushButton_2);

        pushButton = new QPushButton(RobotEditor);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setFont(font);

        horizontalLayout->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(RobotEditor);

        QMetaObject::connectSlotsByName(RobotEditor);
    } // setupUi

    void retranslateUi(QDialog *RobotEditor)
    {
        RobotEditor->setWindowTitle(QApplication::translate("RobotEditor", "Robot Editor", nullptr));
        pushButton_2->setText(QApplication::translate("RobotEditor", "Load", nullptr));
        pushButton->setText(QApplication::translate("RobotEditor", "Save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RobotEditor: public Ui_RobotEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROBOTEDITOR_H
