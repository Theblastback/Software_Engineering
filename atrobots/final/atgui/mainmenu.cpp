#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "robotselection.h"
#include "roboteditor.h"
#include "atrlock.h"
#include "QPixmap"
#include "QPalette"
#include "QFontDatabase"

MainMenu::MainMenu(QWidget *parent) :
    QMainWindow(parent),
    mm(new Ui::MainMenu)
{
    mm->setupUi(this);
    QFontDatabase::addApplicationFont(":/fonts/ROGFontsv1.6-Regular.ttf");
    QPixmap bg(":/background/background.png");
    QPalette p;
    p.setBrush(QPalette::Background, bg);
    this->setPalette(p);

    mm->label->setStyleSheet("color: white;");
    mm->label_2->setStyleSheet("color: white;");
}

MainMenu::~MainMenu()
{
    delete mm;
}

void MainMenu::on_pushButton_clicked()
{
    RobotSelection rs;
    rs.setModal(true);
    rs.exec();
    //rs = new RobotSelection(this);
    //rs->show();
}

void MainMenu::on_pushButton_2_clicked()
{
    RobotEditor re;
    re.setModal(true);
    re.exec();
    //re = new RobotEditor(this);
    //re->show();
}

void MainMenu::on_pushButton_3_clicked()
{
    atrlock alock;
    alock.setModal(true);
    alock.exec();
    //lwindow = new atrlock(this);
    //lwindow->show();
}
