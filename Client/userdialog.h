#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>
#include "headdialog.h"
#include "user.h"
#include "headdialog.h"
#include <QTimer>
#include <homedialog.h>

namespace Ui {
class UserDialog;
}

class UserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDialog(QWidget *parent = nullptr);
    ~UserDialog();

    void get_PicMessage();
    void changeUserBtn();
//    void changeUserBtn_one();
//    void changeUserBtn_two();
//    void changeUserBtn_three();
//    void changeUserBtn_four();
//    void changeUserBtn_five();

private slots:
    void on_changeBtn_clicked();

    void on_offBtn_clicked();

    void on_back_home_Btn_clicked();

private:
    Ui::UserDialog *ui;
    QTcpSocket *tcpSocket;
    QTimer *new_timer;

};

#endif // USERDIALOG_H
