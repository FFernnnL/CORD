#ifndef GROUPCHATDIALOG_H
#define GROUPCHATDIALOG_H

#include <QDialog>
#include<QTcpSocket>
#include<QTimer>

namespace Ui {
class GroupchatDialog;
}

class GroupchatDialog : public QDialog
{
    Q_OBJECT

public:
    void getchathistory(QString judge);
    explicit GroupchatDialog(QWidget *parent = nullptr);
    ~GroupchatDialog();

private slots:
    void on_back_Btn_clicked();

    void on_sendBtn_clicked();

private:
    Ui::GroupchatDialog *ui;
    QTcpSocket *tcpSocket;
    QTimer *timer;
};

#endif // GROUPCHATDIALOG_H
