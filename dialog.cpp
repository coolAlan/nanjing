#include "dialog.h"
#include "ui_dialog.h"
#include <QAbstractButton>
#include <QDebug>
#include <QFile>
#include <mainwindow.h>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //qDebug()<<MainWindow::filename<<MainWindow::deviceid;

}

Dialog::~Dialog()
{
    delete ui;


}




void Dialog::on_buttonBox_clicked(QAbstractButton *button)
{

    //qDebug()<<button->text();
    if (button->text() == tr("&OK"))
    {
        QFile examiner("pid.txt");
        examiner.open(QIODevice::WriteOnly|QIODevice::Append);
        QTextStream out(&examiner);
        out<<"#"<<ui->lineEdit->text();
        examiner.close();


    }
    else if (button->text() == tr("Reset"))
    {
        ui->lineEdit->setText(tr("东南大学\x20信息安全研究中心"));
    }
}
