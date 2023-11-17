#ifndef REGISTDIALOG_H
#define REGISTDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include"logindialog.h"

namespace Ui {
class RegistDialog;
}

class RegistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegistDialog(QWidget *parent = nullptr);
    
    ~RegistDialog();

private slots:
    void on_okBtn_clicked();
    void readData();
    void on_backBtn_clicked();

private:
    Ui::RegistDialog *ui;
    QTcpSocket *tcpSocket;
    QString username;
    QString password_1,password_2;

};

#endif // REGISTDIALOG_H
