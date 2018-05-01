#ifndef ROBOTSELECTION_H
#define ROBOTSELECTION_H

#include <QDialog>

namespace Ui {
class RobotSelection;
}

class RobotSelection : public QDialog
{
    Q_OBJECT

public:
    explicit RobotSelection(QWidget *parent = 0);
    ~RobotSelection();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::RobotSelection *ui;
};

#endif // ROBOTSELECTION_H
