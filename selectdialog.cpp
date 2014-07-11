#include "selectdialog.h"
#include "ui_selectdialog.h"
#include <QProcess>
#include <QStringList>
#include <QStringListModel>
#include <QDir>
#include <QDate>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QCheckBox>
#include <QSettings>
#include <QDebug>



selectDialog::selectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::selectDialog)
{
    ui->setupUi(this);
    QProcess *shellprocess = new QProcess(this);
    ui->progressBar->hide();
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setCurrentSection(QDateEdit::DaySection);

    QSettings *settings = new QSettings ("config.ini", QSettings ::IniFormat);
    adbPath = settings->value("path/adbPath","/home/gaoyue/adt-bundle-linux-x86/sdk/platform-tools/").toString();
    system((adbPath + tr("adb kill-server")).toStdString().c_str());
    system((tr("")+adbPath + tr("adb start-server")).toStdString().c_str());

    shellprocess->start(adbPath+tr("adb devices"));
    shellprocess->waitForFinished();

    QString tmp = shellprocess->readAllStandardOutput();
    tmplist = tmp.split('\n',QString::SkipEmptyParts);
    //QStringList tmplist2 = tmplist.filter(tr("emulator"));

    tmplist.removeAll(tr("emulator-5554	device"));
    tmplist.removeAll(tr("emulator-5554	offline"));
    tmplist.removeAll(tr("* daemon not running. starting it now on port 5037 *"));
    tmplist.removeAll(tr("* daemon started successfully *"));
    tmplist.removeAll(tr("List of devices attached "));
    flag = 0;


    QStringListModel *model = new QStringListModel(this);
    model->setStringList(tmplist);
    ui->listView->setModel(model);
    if(shellprocess) {shellprocess->close();}
    ui->listView->setWrapping(true);
    dirName = tr("example");


}

selectDialog::~selectDialog()
{
    delete ui;
    if(flag) pullapk.terminate();

}

void selectDialog::on_listView_clicked(const QModelIndex &index)
{

    serial = index.data().toString().split('	').at(0);
     ui->pushButton->setText(tr("选择设备"));
     ui->progressBar->setValue(0);

     QProcess *subprocess = new QProcess(this);
     subprocess->start(adbPath+tr("adb -s ")+serial+tr(" shell getprop ro.product.model"));
     subprocess->waitForFinished();


     ui->lineEdit_2->setText(subprocess->readLine());
     subprocess->terminate();


}

QString selectDialog::deBlank(QString strs)
{
    int len = strs.length();
    QString strnew;
    for (int i=0;i<len;i++)
    {

        if (!strs.at(i).isSpace())
        {
            strnew.append(strs[i]);
        }
    }
    return strnew;
}


void selectDialog::on_pushButton_clicked()
{
    if (ui->lineEdit->text().isEmpty()||ui->lineEdit_2->text().isEmpty()) {
        QMessageBox::about(NULL, "Error", "请填写送检单位和手机型号        ");
        return;
    }

    dirName = deBlank(ui->dateEdit->date().toString("yyyyMMdd")+ui->lineEdit->text()+ui->lineEdit_2->text());

    //QString dir = QApplication::applicationDirPath();
    if (serial.isNull() && !tmplist.isEmpty()) {serial = tmplist.at(0).split('	').at(0);}
///////////////////use ls to list apk filename(need rooted devices)///////////////////
/*
      pullapk.setStandardInputFile("scripts/backup.txt");
       pullapk.start(tr("adb -s ")+serial +tr(" shell"));
       pullapk.waitForFinished();

    QStringList apklist  = QString(pullapk.readAllStandardOutput()).split("\n");
    apklist.removeLast();
    if (apklist.isEmpty()) {
        system((tr("notify-send -i ") + dir + tr("/scripts/error.png \"无法连接移动设备\"")).toStdString().c_str());
        return;
    }
    qDebug()<<apklist;

    QList<QString>::Iterator it = apklist.begin(),itend = apklist.end();
    QString ittmp;


    QDir *temp = new QDir;
    bool exist = temp->exists("./sample/"+dirName);
    flag = 1;
    if(!exist)
    {
        temp->mkdir("./sample/"+dirName);
    }
    ui->pushButton->setText(tr("正在导入"));
    //ui->pushButton_2->setText(tr("停止"));
    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(0);
    int counter = 0;
    int total = apklist.count();
    for (;it != itend; it++,counter++){
    QCoreApplication::processEvents();
        ittmp = QString(*it);
         ui->progressBar->setValue(counter*100/total);
         if (ittmp.contains("data/app/")& !ittmp.contains("ls /data/app/*.apk"))
         {
           // QString cmd = tr("adb -s ")+ serial + tr(" pull ")+ittmp + tr(" sample |notify-send -i ")+dir+tr("/scripts/apk.png Copying ")+ittmp;
            pullapk.start(tr("adb -s ")+ serial + tr(" pull ")+ittmp + tr(" sample/")+dirName);
            //system((tr("notify-send -i ")+dir+tr("/scripts/apk.png Copy ")+ittmp).toStdString().c_str());
            pullapk.waitForFinished(60000);

         }
     }

    ui->progressBar->setValue(100);
    ui->pushButton->setText(tr("导入完成"));
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setText(tr("关闭"));
    system((tr("notify-send -i ")+ dir +tr("/scripts/done.jpg \" 文件导入完成\"")).toStdString().c_str());

    pullapk.terminate();
*/

///////////////////use packages.xml to acquire apk filename///////////////////
    QDir *temp = new QDir;
    bool exist = temp->exists("./sample/"+dirName);
    flag = 1;
    if(!exist)
    {
        temp->mkdir("./sample/"+dirName);
    }
    QString cmd = adbPath+tr("adb -s ")+ serial + tr(" pull /data/system/packages.xml sample/")+dirName;
    pullapk.start(cmd);
    pullapk.waitForFinished();
    //system((tr("notify-send -i ")+ dir +tr("/scripts/done.jpg \" 导入packages.xml完成\"")).toStdString().c_str());

    QString srcFileName = tr("sample/")+dirName+tr("/packages.xml");

    QFile srcFile(srcFileName);
     if (!srcFile.open(QFile::ReadOnly | QFile::Text)) {
         QMessageBox::warning(this, tr("Warning/ Convert"),
                           tr("Cannot open soruce file %1:\n%2.")
                           .arg(srcFileName)
                           .arg(srcFile.errorString()));
         QMessageBox message(QMessageBox::Warning,"Information","Your phone's Andorid version is higher than 2.X. \n If your phone is ROOTed,please click Yes ,else No",QMessageBox::Yes|QMessageBox::No,NULL);
             if (message.exec()==QMessageBox::Yes)
             {
                 QProcess *subpro = new QProcess(this);
                 pullapk.start("sh getXml.sh "+serial+" "+dirName);
                 pullapk.waitForFinished();
                 //pullapk.terminate();
             }
             else
             {
                 return;
             }

     }
     if (!srcFile.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::about(NULL, "error","You phone is not ROOTed!");
        return;
     }
     QXmlStreamReader reader(&srcFile);
     apkpath.clear();

     while (!reader.atEnd())
     {
         reader.readNext();
         if(reader.name()=="package" && !reader.isEndElement())
         {
             //qDebug()<<reader.attributes().at(0).value();
             //qDebug()<<reader.attributes().at(1).value();
             if (!reader.attributes().at(1).value().startsWith(tr("/system")))
             {
                 apkpath.insert(reader.attributes().at(0).value().toString(),reader.attributes().at(1).value().toString());
             }
         }

     }
     srcFile.close();

    // qDebug()<<apkpath;

     ui->tableWidget->setColumnCount(3);
     ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("")<<tr("APK")<<tr("PATH"));
     ui->tableWidget->setRowCount(apkpath.count());
     ui->tableWidget->setColumnWidth(0,30);
     ui->tableWidget->setColumnWidth(1,200);
     ui->tableWidget->setColumnWidth(2,250);

     for(int i=0;i<apkpath.count();i++)
         {
            QTableWidgetItem *checkBox = new QTableWidgetItem();
            checkBox->setCheckState(Qt::Unchecked);
            ui->tableWidget->setItem(i,0,checkBox);
            QTableWidgetItem *item=new QTableWidgetItem((apkpath.begin()+i).key());
            ui->tableWidget->setItem(i,1,item) ;
            item=new QTableWidgetItem((apkpath.begin()+i).value());
            ui->tableWidget->setItem(i,2,item) ;

        }

    flag = 0;
}

void selectDialog::on_pushButton_2_clicked()
{
    if(flag) {pullapk.terminate();}
    selectDialog::close();

}

void selectDialog::on_pushButton_3_clicked()
{
    int total=0;
    if (apkpath.isEmpty()){QMessageBox::about(NULL, "Error", "导入APK前请先选择设备     ");return;}
    for(int i=0;i<apkpath.count();i++)
    {
        if (ui->tableWidget->item(i,0)->checkState()) total++;
    }
    if (!total)QMessageBox::about(NULL, "Error", "您未选择需要导入的APK     ");
    else
    {
        ui->progressBar->setVisible(true);
        ui->progressBar->setValue(0);
        int counter=0;
        for(int i=0;i<apkpath.count();i++)
        {
            if (ui->tableWidget->item(i,0)->checkState())
             {
                QString cmd = adbPath+tr("adb -s ")+ serial + tr(" pull ")+ui->tableWidget->item(i,2)->text() + tr(" sample/")+dirName+tr("/")+ui->tableWidget->item(i,1)->text()+tr(".apk");
                pullapk.start(cmd);
                pullapk.waitForFinished();

                counter++;
                ui->progressBar->setValue(counter*100/total);
                //qDebug()<<cmd;
            }

        }
        ui->progressBar->setValue(100);
        system((tr("notify-send -i ")+ QApplication::applicationDirPath() +tr("/scripts/done.jpg \" APK导入完成\"")).toStdString().c_str());
    }


}

void selectDialog::on_pushButton_4_clicked()
{
    if (apkpath.isEmpty()){QMessageBox::about(NULL, "Error", "导入APK前请先选择设备     ");return;}
    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(0);
    for(int i=0;i<apkpath.count();i++)
    {


            QString cmd = adbPath+tr("adb -s ")+ serial + tr(" pull ")+ui->tableWidget->item(i,2)->text() + tr(" sample/")+dirName+tr("/")+ui->tableWidget->item(i,1)->text()+tr(".apk");
            pullapk.start(cmd);
            pullapk.waitForFinished();
            ui->progressBar->setValue((i+1)*100/apkpath.count());
            //qDebug()<<cmd;


    }

    ui->progressBar->setValue(100);
    system((tr("notify-send -i ")+ QApplication::applicationDirPath() +tr("/scripts/done.jpg \" APK导入完成\"")).toStdString().c_str());
}
