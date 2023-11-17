#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
#include<QTcpServer>
#include<QTcpSocket>
#include<QDialog>
#include<QDebug>
#include <QMessageBox>
#include "user.h"
#include"homedialog.h"
#include"registdialog.h"



namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void on_loginBtn_clicked();

    void on_regBtn_clicked();
    void readData();

private:
    Ui::LoginDialog *ui;
    QTcpSocket *tcpSocket;
    QString username;
    QString password;
};

#endif // LOGINDIALOG_H
