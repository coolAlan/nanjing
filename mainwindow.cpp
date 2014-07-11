#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"

#include <QDropEvent>
//#include <QKeyEvent>
#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QElapsedTimer>
#include <QMimeData>
#include <QMap>
#include <QSettings>
#include <QTextStream>
#include <QTime>
#include <QCryptographicHash>
#include <QStringList>

////////////////////////////////////////////////////////////////////
/////////   difine common path and AVD name in config.ini  /////////
////////////////////////////////////////////////////////////////////

//#define pythonPath  "/usr/bin/"
//#define emulatorPath "/home/gaoyue/adt-bundle-linux-x86/sdk/tools/"
//#define adbPath "/home/gaoyue/adt-bundle-linux-x86/sdk/platform-tools/"

//#define avdName "AVD"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    //read file config.ini
    QSettings *settings = new QSettings ("config.ini", QSettings ::IniFormat);
    //pythonPath = settings->value("path/pythonPath","/usr/bin/").toString();
    emulatorPath = settings->value("path/emulatorPath","/home/gaoyue/adt-bundle-linux-x86/sdk/tools/").toString();
    adbPath = settings->value("path/adbPath","/home/gaoyue/adt-bundle-linux-x86/sdk/platform-tools/").toString();
    javaPath = settings->value("path/javaPath","/opt/jdk1.7.0_45/bin/").toString();
    avdName = settings->value("path/avdName","AVD").toString();
    wipedata = settings->value("path/wipedata","1").toInt();
    ui->setupUi(this);

    //start Android emulator
    //need export 'emulator' and 'adb' into environmental $PATH first
    emuflag = 0;
    startemu();

    //delay 3s
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<3000) QCoreApplication::processEvents();

    //setWindowOpacity(0.9);

    QLabel *permanent = new QLabel(this);
    permanent->setFrameStyle(QFrame::Box|QFrame::Sunken);
    permanent->setText(tr("东南大学\x20信息安全研究中心"));
    ui->statusBar->addPermanentWidget(permanent);
    this->setAcceptDrops(true);  //在父类 mainwindow中启动 托放功能
    /*QFont font;
    //font.setItalic(true);
    font.setWeight(75);
    ui->label->setFont(font);
    ui->label2->setFont(font);
    ui->label3->setFont(font);
    ui->label4->setFont(font);
    ui->label5->setFont(font);
    //ui->label6->setFont(font);*/
    ui->label6->setText(tr("如需导入apk,请先通过usb连接待测手机"));
    ui->pushButton2->setEnabled(false);
    ui->pushButton3->setEnabled(false);
    ui->pushButton5->setEnabled(false);
    ui->pushButton4->setEnabled(false);
    timerflag = 0;
    droidboxflag = 0;processflag = 0;

}

MainWindow::~MainWindow()
{
   delete ui;
   if(emuflag) mainprocess->terminate();

}

/*void MainWindow::keyPressEvent(QKeyEvent *event)
{

    qDebug()<<char(event->key());
   // ui->lineEdit->setText(QString(event->key()));
   // process= new QProcess(this);
   //process->start("/home/gaoyue/adt-bundle-linux-x86/sdk/tools/emulator -avd AVD ");
}*/


///////////////////////////////////////////
//    click "import apk files" Button    //
///////////////////////////////////////////
void MainWindow::on_pushButton6_clicked()
{
    //system((tr("python scripts/pull_apk.py")).toStdString().c_str());

    ui->label6->setText(tr("正在导入apk,请耐心等待"));
    selectDialog j;
    j.setModal(true);
    j.exec();
    ui->label6->setEnabled(true);
    ui->label6->setText(tr("请选择apk开始测试"));
    ui->pushButton2->setEnabled(false);
    ui->pushButton3->setEnabled(false);
    ui->pushButton4->setEnabled(false);
}

///////////////////////////////////////////
//     click "Open Apk File" Button      //
///////////////////////////////////////////
void MainWindow::on_pushButton_clicked()
{


        filepath = QFileDialog::getOpenFileName(this,tr("请选择测试apk"),"./sample",tr("*.apk"));

        ui->pushButton2->setDown(false);
        ui->pushButton3->setDown(false);
        ui->pushButton5->setDown(false);

    if(filepath==NULL)
    {
        ui->label->setText(tr("未选择apk文件"));
        ui->pushButton2->setEnabled(false);
        ui->pushButton3->setEnabled(false);
        ui->pushButton4->setEnabled(false);
        ui->label2->setText("");
        ui->label3->setText("");
        ui->label4->setText("");
        ui->label5->setText("");
        return;
    }


        // check apk_list
        QFile theFile(filepath);
        theFile.open(QIODevice::ReadOnly);
        QByteArray md5 = QCryptographicHash::hash(theFile.readAll(),QCryptographicHash::Md5);
        theFile.close();
        res = md5.toHex().constData();
        //qDebug() << res;


        QFile apklist("apk_list");
        apklist.open(QIODevice::ReadOnly);
        QByteArray apk_list = apklist.readAll();
        apklist.close();




            filename = filepath.section('/',-1);
            QStringList pathlist = filepath.split('/');
            deviceid = pathlist.at(pathlist.count()-2);

            ui->pushButton2->setEnabled(true);
            ui->pushButton3->setEnabled(true);
            if(apk_list.contains(res)){
                ui->label->setText(filename+tr("已测试，可查看历史记录"));
                ui->label2->setText("");
                ui->label3->setText("");
            }
            else {
                ui->label->setText(filename);
                ui->label2->setText(tr("模拟器启动过程中，可以先进行静态分析"));
                ui->label3->setText(tr("请等待模拟器启动完成后进行测试"));
            }
            ui->label4->setText("");
            ui->label5->setText("");
            filepath = "\""+filepath+"\"";
            ui->pushButton5->setEnabled(false);



}

///////////////////////////////////////////
//     click "Static Analysis" Button    //
///////////////////////////////////////////
void MainWindow::on_pushButton2_clicked()
{
    ui->statusBar->showMessage(tr("   正在进行静态分析..."));
    ui->pushButton2->setDown(true);
    ui->pushButton3->setDown(false);
    ui->pushButton5->setDown(false);
    ui->label2->setEnabled(true);
    ui->label2->setText(tr("正在反编译apk "));
    ui->label3->setText(tr("请勿同时进行静态和动态分析"));
    behaviortest();
    ui->pushButton4->setEnabled(true);

}

///////////////////////////////////////////
//     click "Dynamic Analysis" Button      //
///////////////////////////////////////////
void MainWindow::on_pushButton3_clicked()
{
    ui->statusBar->showMessage(tr("   正在安装应用程序..."));
    ui->pushButton2->setDown(false);
    ui->pushButton3->setDown(true);
    ui->pushButton5->setDown(false);
    droidbox();
    ui->pushButton4->setEnabled(true);
    ui->pushButton5->setEnabled(true);
    //ui->label5->setText(tr("请在应用程序启动后点击按钮"));
    ui->statusBar->showMessage(tr("   正在进行动态分析..."));
}

///////////////////////////////////////////
//     click "Generate Report" Button    //
///////////////////////////////////////////
void MainWindow::on_pushButton4_clicked()
{
    Dialog h;
    h.setModal(true);
    h.exec();
    ui->statusBar->showMessage(tr("   生成报告..."));
    ui->pushButton2->setDown(false);
    ui->pushButton3->setDown(false);
    ui->pushButton4->setDown(false);
    ui->label3->setText("");
    ui->label5->setText("");
    ui->pushButton5->setText("自动化测试");
    //delay 3 seconds
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<1000) QCoreApplication::processEvents();

    output();
}

///////////////////////////////////////////
//     click "Automatic Test" Button     //
///////////////////////////////////////////
void MainWindow::on_pushButton5_clicked()
{

    ui->pushButton2->setDown(false);
    ui->pushButton3->setDown(false);
    ui->pushButton5->setDown(true);
     ui->label3->setText("");
     if (processflag == 0)
     {
         autotest();
         processflag = 1;
         ui->statusBar->showMessage(tr("   正在进行自动化测试，请勿操作"));
         ui->pushButton5->setText(tr("手工测试"));
     }
     else {
         ui->label5->setText(tr("自动化测试已停止,可以进行手工测试"));
         ui->pushButton5->setText(tr("自动化测试"));
         lastprocess->terminate();
         processflag = 0;
     }
}


///////////////////////////////////////////
//     click "Check History" Button     //
///////////////////////////////////////////
void MainWindow::on_pushButton7_clicked()
{
    ui->pushButton2->setDown(false);
    ui->pushButton3->setDown(false);
    ui->pushButton4->setDown(false);
    historyDialog k;
    k.setModal(true);
    k.exec();
}
///////////////////////////////////////////
//     Start Android Vitual Device       //
///////////////////////////////////////////
void MainWindow::startemu()
{
    ui->statusBar->showMessage(tr(" 模拟器正在启动，请耐心等待..."));

    mainprocess= new QProcess(this);

    QString cmd;
    if (wipedata == 0)
         cmd= emulatorPath + tr("emulator -avd ") + avdName + tr(" -system images/system.img -ramdisk images/ramdisk.img -kernel images/zImage -prop dalvik.vm.execution-mode=int:portable ");
    else
         cmd= emulatorPath + tr("emulator -avd ") + avdName + tr(" -system images/system.img -ramdisk images/ramdisk.img -kernel images/zImage -wipe-data -prop dalvik.vm.execution-mode=int:portable ");

    mainprocess->start(cmd);
    emuflag = 1;
    mainprocess->waitForStarted();
    qDebug()<<cmd;


}

///////////////////////////////////////////
//         Start Static Analysis         //
///////////////////////////////////////////
void MainWindow::behaviortest()
{
    QFile file1("api_list");
    if (!file1.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->statusBar->showMessage(tr("Error:无法打开 api_list"));
        return;
    }
    QTextStream in(&file1);
    QString api,decription,grep;
    int counter = 0;
    while(1)
    {
    in>>api>>decription;
    if(api=="")break;
    api_map.insert(api,decription);
    //qDebug()<<api<<decription;
    }
    file1.close();


    //decompile selected apk

    //system((tr("./apktool d -f ")+ filepath + tr(" ./apk/")+filename+tr("/decompile")).toStdString().c_str());
    system((javaPath+tr("java -jar ./apktool2.jar d -f ")+ filepath + tr(" -o apk/")+filename+tr("/decompile")).toStdString().c_str());

    file1.setFileName("./apk/"+filename+"/staticResult.txt");
    if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        ui->statusBar->showMessage(tr("Error:无法打开 staticResult.txt"));
        return;
    }
    QTextStream out(&file1);

    stdprocess = new QProcess(this);
    QByteArray apiResult;
    time.start();
    int total = api_map.count();
    int counter2 = 0;

    ui->label2->setText(tr("静态分析中.. 目前发现 0 项敏感API"));
    //grep sensitive
    foreach (api,api_map.keys())
    {

        QCoreApplication::processEvents();
        grep = tr("grep -i -R -w ") + api + tr(" ./apk/") + filename + tr("/decompile/smali --exclude-dir=android");
        counter2++;

        stdprocess->start(grep);
        stdprocess->waitForFinished();
        apiResult = "";
        apiResult = stdprocess->readAllStandardOutput();
        stdprocess->close();
        if(!apiResult.isNull())
        {
           qDebug() << tr("found ") << api << api_map[api];
           //system((tr("notify-send ")+api_map[api]).toStdString().c_str());
           out<<api<< "###" << api_map[api]<<endl<<apiResult<<endl;
           counter++;

           //qDebug()<<apiResult;
        }

        ui->label2->setText(tr("静态分析中(")+QString::number(counter2)+"/"+QString::number(total)+")."+tr(" 目前发现 ") + QString::number(counter) + tr(" 项敏感API"));

     }
    file1.close();

    ui->statusBar->showMessage(" Time Elapsed: "+QString::number(time.elapsed()/1000.0,'f',1)+"s");

    ui->label2->setText(tr("静态分析已完成.. 总计发现 ") + QString::number(counter) + tr(" 项敏感API"));

    system((tr("notify-send -i ")+  QApplication::applicationDirPath() +tr("/scripts/done.jpg \" 静态分析已完成\"")).toStdString().c_str());
    stdprocess->terminate();
    ui->pushButton4->setEnabled(true);
}

///////////////////////////////////////////
//         Start Dynamic Analysis        //
///////////////////////////////////////////
void MainWindow::droidbox()
{
    //system((adbPath+tr("adb install scripts/InsertData.apk")))
    system((adbPath+tr("adb -s emulator-5554 logcat -c")).toStdString().c_str());

    //subprocess= new QProcess(this);

    //QString cmd = adbPath + tr("adb -s emulator-5554 logcat -d -v time dalvikvm:W OSNetworkSystem:W *:S");
    //QString cmd = adbPath + tr("adb -s emulator-5554 logcat -v time -r 10000 -f cache/")+filename+tr(".log dalvikvm:W OSNetworkSystem:W *:S");
    //subprocess->start(cmd);

    subprocess= new QProcess(this);
    QString cmd = adbPath + tr("adb -s emulator-5554 logcat -v time dalvikvm:W OSNetworkSystem:W *:S");

    subprocess->start(cmd);

    thdprocess = new QProcess(this);

    thdprocess->start(tr("python scripts/sdroid.py ")+filepath);
    droidboxflag = 1;
    thdprocess->waitForFinished();

//thdprocess->terminate();

    QDir *temp = new QDir;
    bool exist = temp->exists("./apk/"+filename);
    if(!exist)
    {
        temp->mkdir("./apk/"+filename);
    }

    file.setFileName("./apk/"+filename+"/dynamicResult.txt");
    file.open(QIODevice::WriteOnly);
    file.close();

    time.restart();
    timerID=startTimer(5000);
    timerflag = 1;
    ui->label3->setEnabled(true);
    ui->label3->setText(tr("应用程序安装完成后自动打开，请耐心等待"));


}

void MainWindow::output()
{
 //   file.setFileName("dynamicResult.txt");
 //   if(!file.open(QIODevice::WriteOnly | QFile::Text))
 //   {
 //      ui->statusBar->showMessage(tr("Error:write log"));
       //return;
  //   }
 //   QTextStream out (&file);
 //   out << subprocess->readAllStandardOutput();
 //   file.flush();
    QFile apklist("apk_list");
    apklist.open(QIODevice::WriteOnly|QIODevice::Append);
    QTextStream out(&apklist);
    out<<filepath<<" "<<QString(res)<<"\n";
    apklist.close();


    if (timerflag) { killTimer(timerID);timerflag = 0;}
///////////////////////////////////////////////
/*    subprocess= new QProcess(this);
        QString cmd =  adbPath + tr("adb -s emulator-5554 logcat -d dalvikvm:W OSNetworkSystem:W *:S");
        qDebug()<<cmd;
        subprocess->start(cmd);
        subprocess->waitForFinished();


        QDir *temp = new QDir;
        bool exist = temp->exists("./apk/"+filename);
        if(!exist)
        {
            temp->mkdir("./apk/"+filename);
        }

        file.setFileName("./apk/"+filename+"/dynamicResult.txt");
        if(!file.open(QIODevice::WriteOnly | QFile::Text))
        {
           ui->statusBar->showMessage(tr("Error:write log"));
           return;
         }

        QTextStream out1 (&file);
        out1<< subprocess->readAllStandardOutput();
        //qDebug()<<subprocess->readAllStandardOutput();
        file.flush();

        file.close();
*/
////////////////////////////////////////

    if (droidboxflag) {
            subprocess->kill();
            thdprocess->terminate();
            droidboxflag = 0;
            ui->statusBar->showMessage(" Time Elapsed: "+ QString::number(time.elapsed()/1000.0,'f',1)+"s");
            //QDir *temp = new QDir;
            //bool exist = temp->exists("./apk/"+filename);
            //if(!exist)
            //{
            //    temp->mkdir("./apk/"+filename);
            //}
            //system((adbPath+tr("adb -s emulator-5554 pull cache/")+filename+tr(".log apk/")+filename+tr("/dynamicResult.txt")).toStdString().c_str());
            //system((adbPath+tr("adb -s emulator-5554 shell rm cache/")+filename+tr(".log")).toStdString().c_str());
    }
    if (processflag)   { lastprocess->terminate(); processflag = 0;}
    system((tr("python scripts/report.py \"")+ filename+tr("\" \"")+deviceid+tr("\"")).toStdString().c_str());
    ui->label4->setEnabled(true);
    ui->label4->setText(tr("测试完成，报告已保存至report文件夹"));

}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
    else event->ignore();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList <QUrl> urlList=event->mimeData()->urls();
    filepath = urlList.at(0).toLocalFile();
    ui->label->setText(filepath.section('/',-1));
    filepath = "\""+filepath+"\"";
    ui->pushButton2->setEnabled(true);
    ui->pushButton3->setEnabled(true);
}

void MainWindow::timerEvent(QTimerEvent *)
{
    //qDebug()<<"20s";


    QDir *temp = new QDir;
    bool exist = temp->exists("./apk/"+filename);
    if(!exist)
    {
        temp->mkdir("./apk/"+filename);
    }

    file.setFileName("./apk/"+filename+"/dynamicResult.txt");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text))
    {
       ui->statusBar->showMessage(tr("Error:write log"));
       return;
     }

    QTextStream out (&file);
    QString text = subprocess->readAllStandardOutput();
    if(text.endsWith(tr("Invalid argument\r\n")))
    {
        qDebug()<<"!!";
        system((adbPath+tr("adb -s emulator-5554 logcat -c")).toStdString().c_str());
        subprocess->terminate();
        subprocess->start(adbPath + tr("adb -s emulator-5554 logcat -v time dalvikvm:W OSNetworkSystem:W *:S"));
    }
    else out << text;
    file.flush();
    file.close();
    //subprocess->terminate();
}

void MainWindow::autotest()
{
    lastprocess = new QProcess(this);
    lastprocess->start(tr("python scripts/autoTest.py "));


    ui->label5->setEnabled(true);
    ui->label5->setText(tr("自动化测试中,测试完成后程序将自动退出\n若程序长时间无响应，请点击手工测试"));


}



