#ifndef SELECTDIALOG_H
#define SELECTDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QAbstractButton>
#include <QMap>
#include <QModelIndex>
namespace Ui {
class selectDialog;
}

class selectDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit selectDialog(QWidget *parent = 0);
    ~selectDialog();
    QString deBlank(QString strs);
    
private slots:
    void on_listView_clicked(const QModelIndex &index);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::selectDialog *ui;
    QString serial;
    QStringList tmplist;
    int flag;
    QProcess pullapk;
    QMap <QString,QString>apkpath;
    QString dirName;
    QString adbPath;

};

#endif // SELECTDIALOG_H
