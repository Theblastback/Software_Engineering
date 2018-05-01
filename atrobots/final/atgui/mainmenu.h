#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMainWindow>
#include "robotselection.h"
#include "roboteditor.h"
#include "atrlock.h"

namespace Ui {
class MainMenu;
}

class MainMenu : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = 0);
    ~MainMenu();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainMenu *mm;
    //RobotSelection *rs;
    //RobotEditor *re;
    //atrlock *lwindow;
};

#endif // MAINMENU_H
