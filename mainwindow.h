#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QMap>
#include <QFile>
#include <QTime>
#include "dialog.h"
#include "selectdialog.h"
#include "historydialog.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void startemu();
    void droidbox();
    void behaviortest();
    void autotest();
    void output();

private:
    Ui::MainWindow *ui;
    QProcess *mainprocess,*subprocess,*stdprocess,*thdprocess,*lastprocess;
    QString filepath,filename,deviceid;
    QString emulatorPath,adbPath,javaPath,avdName;
    QMap<QString,QString> api_map;
    QFile file;
    QTime time;
    int timerID,timerflag,processflag,droidboxflag,emuflag,wipedata;
    QByteArray res;
    //Dialog h;
    //selectDialog j;

protected:
    //void keyPressEvent(QKeyEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent *);
    void timerEvent(QTimerEvent *);

private slots:
    void on_pushButton_clicked();
    void on_pushButton3_clicked();
    void on_pushButton2_clicked();
    void on_pushButton4_clicked();
    //void StdOut();
    void on_pushButton5_clicked();
    void on_pushButton6_clicked();
    void on_pushButton7_clicked();
};

#endif // MAINWINDOW_H
