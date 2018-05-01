#include "atrlock.h"
#include "ui_atrlock.h"
#include "filelib.h"
#include "QFileDialog"
#include "QFile"
#include "QFileInfo"
#include <ctime>
#include <iostream>
#include <string>
#include <QMessageBox>

#define LOCKTYPE 3;

string fn1, fn2;
fstream f1, f2;
string s, s1, s2, lock_code;
int16_t i, j, k, lock_pos, lock_dat, this_dat;

using namespace std;

string copy(string name, uint16_t start, uint16_t amount) {
    string temp = "";

    temp = name.substr(start, amount);

    return temp;
}

string lstr(string s1, uint16_t l) {
    if (s1.length() <= l)
        return s1;
    else
        return (copy(s1, 0, l));
}

std::string base_name(std::string name) {
    uint16_t k;
    string s1;

    s1 = "";

    k = 0;

    while ((k <= name.length()) && (name.at(k) != '.')) {
        s1 = s1 + name.at(k);
        k++;
        if (k == name.length())
            break;
    }

    return s1;
}

string no_path(string fn) {

    return (fn.substr(fn.find_last_of("/\\")+1));

}

string ltrim(string s1) {
    while (s1.size() && isspace(s1.front()))
        s1.erase(s1.begin());

    return s1;
}

string rtrim(string s1) {
    while (!s1.empty() && isspace(s1.at(s1.size() - 1)))
        s1.erase(s1.end() - 1);

    return s1;
}

string btrim(string s1) {

    s1 = ltrim(s1);
    s1 = rtrim(s1);

    return s1;
}

string ucase(string s) {
    for (uint32_t i = 0; i < s.length(); i++)
        s.at(i) = toupper(s.at(i));

    return s;
}

string encode(string s) {
    int16_t	i;
    static int count = 1;
    if (lock_code.compare("")) {
        for (i = 0; i < s.length(); i++) {
            lock_pos++;
            if (lock_pos >= lock_code.length()) {
                lock_pos = 0;
            }

            if (((s[i] >= 0) && (s[i] <= 31)) || ((s[i] >= 128) && (s[i] <= 255))) {
                s[i] = ' ';
            }

            this_dat = ((unsigned char)s[i]) & 15;
            s[i] = char((((unsigned char) s[i]) ^ (lock_code[lock_pos] ^ lock_dat)) + 1);
            lock_dat = this_dat;
        }
    }
    return s;
}

string prepare(string s, string s1) {
    int16_t i, k;
    string s2;
    // remove comments
    if ((s1.length() == 0) || (s1[0] == ';')) {
        s1 = "";
    }
    else {
        k = 0;

        for (i = (s1.length()-1); i >= 0; i--) {
            if (s1[i] == ';')
                k = i+1;
        }

        if (k > 0)
            s1 = lstr(s1, k - 1);
    }

    // remove excess spaces
    s2 = "";
    for (i = 0; i < s1.length(); i++) {
        if (!((s1[i] == ' ') || (s1[i] == 8) || (s1[i] == 9) || (s1[i] == 10) || (s1[i] == ','))) {
            s2 = s2 + s1[i];
        }
        else {
            if (s2.compare("")) {
                s = s + s2 + " ";
                s2 = "";
            }
        }
    }
    if (s2.compare(""))
        s = s + s2;
    return s;
}

void write_line(string s, string s1) {
    s = prepare(s, s1);
    if (s.length() > 0) {
        s = encode(s);
        f2 << s << endl;
    }
}

atrlock::atrlock(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::atrlock)
{
    ui->setupUi(this);
    QPixmap bg(":/background/background.png");
    QPalette p;
    p.setBrush(QPalette::Background, bg);
    this->setPalette(p);

    ui->label->setStyleSheet("color: white;");
    ui->progressBar->setStyleSheet("color: white;");
}

atrlock::~atrlock()
{
    delete ui;
}

void atrlock::on_selectRobot_clicked()
{
    fn1 = QFileDialog::getOpenFileName(this, "Select Robot to Edit", "").toStdString();
    fn2 = fn1;
    QFile f(QString::fromStdString(fn2));
    QFileInfo fInfo(f.fileName());
    ui->rob_name->setText(fInfo.baseName());
    fn2 = (fInfo.path() + "/" + fInfo.completeBaseName() + ".ATL").toStdString();
}

void atrlock::on_lockRobot_clicked()
{

    time_t cur_time = time(NULL);
    tm *date = localtime(&cur_time);

    lock_pos = -1;
    lock_dat = 0;

    f1.open(fn1, fstream::in);
    f2.open(fn2, fstream::out);

    ui->progressBar->setValue(10);

    //Copy comment header
    f2 << ";------------------------------------------------------------------------------" << endl;
    s = "";
    while ((!f1.eof()) && (!s.compare(""))) {
        std::getline(f1, s);
        s = btrim(s);

        if (s[0] == ';') {
            f2 << s << endl;
            s = "";
        }
    }
    ui->progressBar->setValue(20);
    // Lock the header
    string temp5 = base_name(fn1);

    f2 << ";------------------------------------------------------------------------------" << endl;
    f2 << "; " << no_path(temp5) << " Lock on " << 1 + date->tm_mon << "/" << date->tm_mday << "/" << 1900 + date->tm_year << endl;
    f2 << ";------------------------------------------------------------------------------" << endl;

    lock_code = "";

    for (i = 0; i <= k; i++) {
        lock_code = lock_code + char((rand() % 33) + 65);
    }

    f2 << "#LOCK " << 3 << " " << lock_code << endl;

    ui->progressBar->setValue(35);

    //Decode lock code
    for (i = 0; i < lock_code.length(); i++) {
        lock_code[i] = char(lock_code[i] - 65);
    }

    ui->progressBar->setValue(50);

    std::cerr<< "Encoding " << fn1 << "...";

    // Encode robot
    s = btrim(s);
    if (s.length() > 0)
        write_line("", ucase(s));
    while (!f1.eof()) {
        std::getline(f1, s1);
        s = "";
        s1 = btrim(ucase(s1));
        write_line(s, s1);
    }

    ui->progressBar->setValue(75);

    f1.close();
    f2.close();

    ui->progressBar->setValue(100);
    ui->rob_name->setStyleSheet("color: green;");
    QMessageBox::warning(this, "SUCCESS", "Robot is fully locked!");
}
