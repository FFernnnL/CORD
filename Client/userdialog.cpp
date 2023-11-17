#include "userdialog.h"
#include "ui_userdialog.h"
#include "headdialog.h"
#include "logindialog.h"
#include "user.h"
#include "headdialog.h"
#include "homedialog.h"
#include <QTimer>
#include <QMessageBox>
#include <QTcpSocket>
#include <QTcpServer>

extern user myuser;
extern QString hostip;
extern int hostport;


QString  num;

UserDialog::UserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserDialog)
{
    ui->setupUi(this);

    //展示用户名和问候，更改颜色和字体
    ui->hilabel->setText("Hello! "+myuser.name);ui->hilabel->setText("您好，" + myuser.name);
    ui->hilabel->setStyleSheet("color:white;");
    QFont font("YouYuan",15);
    ui->label->setFont(font);

    //设置背景颜色
    ui->label->setStyleSheet("QLabel{background-image:url(/mnt/hgfs/SharingFileVM/theme/star/user.png);}");
    ui->label->lower();
    ui->label->setGeometry(0,0,320,384);

    //initialization
    get_PicMessage();

}

UserDialog::~UserDialog()
{
    new_timer->stop();
    delete ui;
}

void UserDialog::on_changeBtn_clicked(){//点击跳转到更换头像界面
    this->close();
    HeadDialog *head = new HeadDialog();
    head->show();
}

void UserDialog::get_PicMessage(){//获取头像信息
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
        QString message = QString("get_flag--%1").arg(myuser.name);
        tcpSocket->write(message.toUtf8());
        tcpSocket->flush();
        connect(tcpSocket,
                &QTcpSocket::readyRead,
                [=](){
            QByteArray buffer = tcpSocket->readAll();
            num = QString(buffer);
            if(num=="get_flag_error")
            {
                QMessageBox::warning(this, "Warning!", "获取失败！", QMessageBox::Yes);
            }
            else{
                changeUserBtn();
            }
        });
    }
}

void UserDialog::changeUserBtn(){//更换个人信息界面头像
    ui->changeBtn->setIcon(QIcon(QString("/mnt/hgfs/SharingFileVM/pic/%1.png").arg(num)));
    ui->changeBtn->setIconSize(QSize(ui->changeBtn->width(),ui->changeBtn->height()));

}

void UserDialog::on_offBtn_clicked(){//点击退出登录
    LoginDialog *ldg = new LoginDialog;
    ldg->show();
}

void UserDialog::on_back_home_Btn_clicked(){//点击返回主界面
    this->close();
    HomeDialog *h = new HomeDialog();
    h->show();
}
