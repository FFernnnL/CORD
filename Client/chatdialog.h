#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include<QTimer>
#include<QTcpSocket>
#include <QDialog>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    void getchathistory(QString judge);
    ~ChatDialog();

private slots:
    void on_txtBtn_clicked();



    void on_friend_message_Btn_clicked();



    void on_sendBtn_clicked();

    void on_back_btn_clicked();

    void on_voice_Btn_clicked();

private:
    Ui::ChatDialog *ui;
    QTcpSocket *tcpSocket;
    QTimer *timer;
};

#endif // CHATDIALOG_H
