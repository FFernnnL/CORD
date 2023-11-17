#ifndef HOMEDIALOG_H
#define HOMEDIALOG_H

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QListWidgetItem>
#include <QTimer>

namespace Ui {
class HomeDialog;
}

class HomeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HomeDialog(QWidget *parent = nullptr);
    void create_friendlist();
    void get_PicMessage_2();
    void creat_groupchat_list();
    void changeHomeBtn();
    ~HomeDialog();


private slots:
    void on_ownBtn_clicked();

    void on_searchBtn_clicked();

    void on_friendlistWidget_itemDoubleClicked();

    void on_freshBtn_clicked();

    void on_groupchat_list_itemDoubleClicked();

    void on_search_group_Btn_clicked();

    void on_creat_group_Btn_clicked();

    void on_robot_Btn_clicked();

private:
    Ui::HomeDialog *ui;
    QTcpSocket *tcpSocket;
    QTimer *timer;
//    QListWidgetItem *item;
};

#endif // HOMEDIALOG_H
