#ifndef ROBOTEDITOR_H
#define ROBOTEDITOR_H

#include <QDialog>

namespace Ui {
class RobotEditor;
}

class RobotEditor : public QDialog
{
    Q_OBJECT

public:
    explicit RobotEditor(QWidget *parent = 0);
    ~RobotEditor();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    // void on_pushButton_3_clicked();

private:
    Ui::RobotEditor *ui;
};

#endif // ROBOTEDITOR_H
