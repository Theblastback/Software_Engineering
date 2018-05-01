#ifndef ATRLOCK_H
#define ATRLOCK_H

#include <QDialog>

namespace Ui {
class atrlock;
}

class atrlock : public QDialog
{
    Q_OBJECT

public:
    explicit atrlock(QWidget *parent = 0);
    ~atrlock();

private slots:
    void on_selectRobot_clicked();

    void on_lockRobot_clicked();

private:
    Ui::atrlock *ui;
};

#endif // ATRLOCK_H
