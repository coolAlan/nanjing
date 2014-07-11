#include "historydialog.h"
#include "ui_historydialog.h"
//#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

historyDialog::historyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::historyDialog)
{
    ui->setupUi(this);
    model = new QStandardItemModel;
    model->setColumnCount(4);    // 设置列数

     //设置列标题
     model->setHeaderData(0,Qt::Horizontal,"设备ID");
     model->setHeaderData(1,Qt::Horizontal,"文件名");
     model->setHeaderData(2,Qt::Horizontal,"时间");
     model->setHeaderData(3,Qt::Horizontal,"测试人员");


    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(0,150);
    ui->tableView->setColumnWidth(1,160);
    ui->tableView->setColumnWidth(2,110);
    ui->tableView->setColumnWidth(3,200);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    QFile file("report/history");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::about(NULL, "Error", "无历史记录        ");
        return;
    }
    QTextStream in(&file);

    QStringList tmp,tmp2;
    tmp = in.readAll().split("\n");
    int counter = tmp.count();

    for(int i=0;i<counter;i++){
        tmp2 = tmp.at(i).split(" | ");

        if (tmp2.count()==4)
        {
            model->setItem(i,0,new QStandardItem(tmp2.at(0)));
            model->setItem(i,1,new QStandardItem(tmp2.at(1)));
            model->setItem(i,2,new QStandardItem(tmp2.at(2)));
            model->setItem(i,3,new QStandardItem(tmp2.at(3)));
        }
    }



    file.close();
    model->sort(3,Qt::DescendingOrder);
}

historyDialog::~historyDialog()
{
    delete ui;
    delete model;
}

void historyDialog::on_tableView_clicked(const QModelIndex &index)
{
    int row = index.row();
    QModelIndex index1 = model->index(row,0);
    deviceId =  model->data(index1).toString();
    index1 = model->index(row,1);
    apkName = model->data(index1).toString();
    index1 = model->index(row,2);
    time = model->data(index1).toString();
    //qDebug()<<deviceId;
    //qDebug()<<apkName;
    //qDebug()<<time;

}

void historyDialog::on_pushButton_clicked()
{
    if (deviceId.isNull())
        deviceId = model->index(0,0).data().toString();
    if (apkName.isNull())
        apkName = model->index(0,1).data().toString();
    if (time.isNull())
        time = model->index(0,2).data().toString();
    QRegExp rx("^2[0-9][0-9][0-9](0[1-9]|1[0-2])([0-2][0-9]|3[0-1])([0-1][0-9]|2[0-3])([0-5][0-9])$");
    //qDebug()<<rx.exactMatch(time);
    if (apkName.endsWith(".apk",Qt::CaseInsensitive)&&rx.exactMatch(time))
        system((tr("firefox report/")+deviceId+"/"+apkName+"_"+time+".html").toStdString().c_str());
}

void historyDialog::on_pushButton_2_clicked()
{
    system(tr("rm report/history").toStdString().c_str());
    system(tr("rm apk_list").toStdString().c_str());
    historyDialog::close();
}

void historyDialog::on_pushButton_3_clicked()
{
    historyDialog::close();
}
