#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class historyDialog;
}

class historyDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit historyDialog(QWidget *parent = 0);
    ~historyDialog();
    
private slots:
    void on_tableView_clicked(const QModelIndex &index);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::historyDialog *ui;
    QString deviceId,apkName,time;
    QStandardItemModel *model;
};

#endif // HISTORYDIALOG_H
