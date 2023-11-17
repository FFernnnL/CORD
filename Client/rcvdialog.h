#ifndef RCVDIALOG_H
#define RCVDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QTcpSocket>
#include <QTcpServer>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QFileDialog>

namespace Ui {
class RcvDialog;
}

class RcvDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RcvDialog(QWidget *parent = nullptr);
    ~RcvDialog();

private:
    Ui::RcvDialog *ui;
    QTcpSocket *tcpSocket;
    QTcpServer *tcpServer;
    QTcpSocket *rcv_tcpSocket;
};

#endif // RCVDIALOG_H
