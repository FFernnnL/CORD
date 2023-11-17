#include "headdialog.h"
#include "ui_headdialog.h"
#include "userdialog.h"
#include "user.h"
#include <QTcpSocket>
#include <QTcpServer>
#include <QMessageBox>
#include "logindialog.h"

extern user myuser;
extern QString hostip;
extern int hostport;

int tmp = 0;

HeadDialog::HeadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HeadDialog)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket();
    ui->headBtn_1->setIcon(QIcon("/mnt/hgfs/SharingFileVM/pic/1.png"));
    ui->headBtn_1->setIconSize(QSize(ui->headBtn_1->width(),ui->headBtn_1->height()));

    ui->headBtn_2->setIcon(QIcon("/mnt/hgfs/SharingFileVM/pic/2.png"));
    ui->headBtn_2->setIconSize(QSize(ui->headBtn_1->width(),ui->headBtn_1->height()));

    ui->headBtn_3->setIcon(QIcon("/mnt/hgfs/SharingFileVM/pic/3.png"));
    ui->headBtn_3->setIconSize(QSize(ui->headBtn_1->width(),ui->headBtn_1->height()));

    ui->headBtn_4->setIcon(QIcon("/mnt/hgfs/SharingFileVM/pic/4.png"));
    ui->headBtn_4->setIconSize(QSize(ui->headBtn_1->width(),ui->headBtn_1->height()));

    ui->headBtn_5->setIcon(QIcon("/mnt/hgfs/SharingFileVM/pic/5.png"));
    ui->headBtn_5->setIconSize(QSize(ui->headBtn_1->width(),ui->headBtn_1->height()));
}

HeadDialog::~HeadDialog()
{
    delete ui;
    tcpSocket->close();
}


//选择头像
void HeadDialog::on_headBtn_1_clicked()
{
    tmp = 1;
}

void HeadDialog::on_headBtn_2_clicked()
{

    tmp = 2;
}

void HeadDialog::on_headBtn_3_clicked()
{

    tmp = 3;
}

void HeadDialog::on_headBtn_4_clicked()
{
    tmp = 4;
}

void HeadDialog::on_headBtn_5_clicked()
{

    tmp = 5;
}


void HeadDialog::on_returnBtn_clicked(){//点击确定更换头像，返回个人信息界面
    tcpSocket = new QTcpSocket();
    tcpSocket->abort();
    tcpSocket->connectToHost(hostip, hostport);
    if(!tcpSocket->waitForConnected(30000))
    {
        QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
        this->close();
        LoginDialog *log = new LoginDialog();
        log->show();
    }
    else{

        QString message = QString("update_flag--%1--%2").arg(myuser.name).arg(QString::number(tmp));
        tcpSocket->write(message.toUtf8());
        tcpSocket->flush();
        connect(tcpSocket,
                &QTcpSocket::readyRead,
                [=](){
            QByteArray buffer = tcpSocket->readAll();
            if( QString(buffer).section("--",0,0) == QString("update_flag_success"))
            {
                QMessageBox::information(this, "Message", "更换成功！", QMessageBox::Yes);
                this->close();
            }
            else if( QString(buffer).section("--",0,0) == QString("update_flag_fail"))
            {
                QMessageBox::warning(this, "Warning!", "更换失败！", QMessageBox::Yes);
            }
        });
    }
    UserDialog *new_u = new UserDialog();
    new_u->show();
}
