#ifndef HEADDIALOG_H
#define HEADDIALOG_H

#include <QDialog>
#include "userdialog.h"
#include "homedialog.h"

namespace Ui {
class HeadDialog;
}

class HeadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HeadDialog(QWidget *parent = nullptr);
    ~HeadDialog();

private slots:
    void on_returnBtn_clicked();

private slots:
    void on_headBtn_1_clicked();

    void on_headBtn_2_clicked();

    void on_headBtn_3_clicked();

    void on_headBtn_4_clicked();

    void on_headBtn_5_clicked();

    //void switch_parameter();

private:
    Ui::HeadDialog *ui;
    QTcpSocket *tcpSocket;
};

#endif // HEADDIALOG_H
