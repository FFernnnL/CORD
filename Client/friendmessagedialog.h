#ifndef FRIENDMESSAGEDIALOG_H
#define FRIENDMESSAGEDIALOG_H

#include<QTcpSocket>
#include<QTcpServer>
#include <QDialog>
#include<QTimer>

namespace Ui {
class FriendMessageDialog;
}

class FriendMessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FriendMessageDialog(QWidget *parent = nullptr);
    ~FriendMessageDialog();

private slots:
    void create_friend_Dialog();

    void on_add_friend_note_Btn_clicked();

    void on_delete_friend_Btn_clicked();

    void on_pushButton_clicked();

private:
    Ui::FriendMessageDialog *ui;
    QTcpSocket* tcpSocket ;
    QTimer *timer;
};

#endif // FRIENDMESSAGEDIALOG_H
