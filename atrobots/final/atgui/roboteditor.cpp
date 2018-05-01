#include "roboteditor.h"
#include "ui_roboteditor.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>

RobotEditor::RobotEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RobotEditor)
{
    ui->setupUi(this);
    QPixmap bg(":/background/background.png");
    QPalette p;
    p.setBrush(QPalette::Background, bg);
    this->setPalette(p);
}

RobotEditor::~RobotEditor()
{
    delete ui;
}

//read
void RobotEditor::on_pushButton_2_clicked()
{
    QString fName = QFileDialog::getOpenFileName(this, "Select Robot to Edit", "");
    QFile f(fName);
    if(!f.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "ERROR", "No Robot Selected");
    }
    QTextStream in(&f);
    QString text = in.readAll();
    ui->plainTextEdit->setPlainText(text);
    f.close();
}

//write
void RobotEditor::on_pushButton_clicked()
{
    QString fName = QFileDialog::getOpenFileName(this, "Select Robot to Overwrite", "");
    QFile f(fName);
    if(!f.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "ERROR", "Robot is not open!");
    }
    QTextStream out(&f);
    QString text = ui->plainTextEdit->toPlainText();
    out << text;
    f.flush();
    f.close();
}
