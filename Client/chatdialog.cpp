#include "chatdialog.h"
#include "ui_chatdialog.h"
#include"user.h"
#include<QMessageBox>
#include<QDateTime>
#include"logindialog.h"
#include"friendmessagedialog.h"
#include<QListWidget>
#include<QListWidgetItem>
#include<QIcon>
#include<QBrush>
#include <QJsonDocument>
#include<QJsonArray>
#include <QJsonObject>
#include <filedialog.h>
#include "audiochat.h"

extern QString hostip;
extern int hostport;
extern user myuser;
extern user other_user;
extern QString name ;

ChatDialog::ChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);

    //设置窗口大小
    this->setFixedSize(508,720);

    //聊天框
    ui->listWidget->setGeometry(0,40,508,608);
    ui->listWidget->setStyleSheet("QListWidget{background-image:url(/mnt/hgfs/SharingFileVM/theme/star/chat2.jpg);}");

    //输入框位置
    ui->inputEdit->setGeometry(80,660,370,30);

    //设置发送键的图片
    ui->sendBtn->setGeometry(455,660,30,30);
    ui->sendBtn->setStyleSheet("QPushButton{background-image:url(/mnt/hgfs/SharingFileVM/theme/star/chat.send.jpg);}");

    //返回键
    ui->back_btn->setGeometry(20,8,150,30);
    ui->back_btn->setText("返回");

    //好友信息键
    ui->friend_message_Btn->setGeometry(350,8,150,30);
    ui->friend_message_Btn->setText("查看好友信息");

    //用户名
    ui->label->setGeometry(194,5,120,30);
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label->setText(name);

    //收发文件键
    ui->txtBtn->setGeometry(5,660,30,30);
    ui->txtBtn->setStyleSheet("QPushButton{background-image:url(/mnt/hgfs/SharingFileVM/theme/star/chat.file.jpg);}");


    //语音通话键
    ui->voice_Btn->setGeometry(40,660,30,30);
    ui->voice_Btn->setStyleSheet("QPushButton{background-image:url(/mnt/hgfs/SharingFileVM/vocal.jpg);}");


    tcpSocket = new QTcpSocket();
    timer = new QTimer();


    //进入聊天页面时就获取所有聊天记录
    getchathistory(QString("all"));

    //每隔1.2秒获取新消息
    timer->start(1200);
    connect(timer,&QTimer::timeout,[=](){
        getchathistory(QString("new"));
    });

}

ChatDialog::~ChatDialog()
{
    delete ui;
    timer->stop();
}

void ChatDialog::getchathistory(QString judge){//获取聊天记录
    tcpSocket->abort();
    tcpSocket->connectToHost(hostip, hostport);
    if(!tcpSocket->waitForConnected(500))
    {

        QMessageBox::warning(this, "Warning!", "get_chat_history_error", QMessageBox::Yes);

    }
    else{
        QString message = QString("chat_history--%1--%2--%3").arg(myuser.name).arg(other_user.name).arg(judge);
        tcpSocket->write(message.toUtf8());
        tcpSocket->flush();
        connect(tcpSocket,&QTcpSocket::readyRead,[=](){
            QByteArray jsondata = tcpSocket->readAll();
            QJsonDocument dataDoc = QJsonDocument::fromJson(jsondata);

            if(!dataDoc.isNull()){
                if(dataDoc.isArray()){

                    QJsonArray dataArray = dataDoc.array();
                    foreach (const QJsonValue &dataValue, dataArray) {

                        if(dataValue.isObject()){
                            QJsonObject dataObject = dataValue.toObject();

                            QString friend_message = dataObject.value("text").toString();
                            QString message_time = dataObject.value("time").toString();                  

                            QString message_name = dataObject.value("sender").toString();
                            QListWidgetItem *item_message = new QListWidgetItem(friend_message);
                            QListWidgetItem *item_name = new QListWidgetItem(message_name+"  "+message_time);
                            QListWidgetItem *item_empty = new QListWidgetItem("");

                            QSize itemSize = item_message->sizeHint();
                            itemSize.setHeight(40);
                            item_message->setSizeHint(itemSize);
                            if(message_name==myuser.name){
                                item_message->setBackground(Qt::lightGray);
                                item_message->setIcon(QIcon(QString("/mnt/hgfs/SharingFileVM/pic/%1.png").arg(myuser.flag_head)));
                            }
                            else{
                                QBrush brush(QColor(122,197,205));
                                item_message->setBackground(brush);
                                item_message->setIcon(QIcon(QString("/mnt/hgfs/SharingFileVM/pic/%1.png").arg(other_user.flag_head)));
                            }
                            ui->listWidget->addItem(item_name);
                            ui->listWidget->addItem(item_message);
                            ui->listWidget->addItem(item_empty);
                        }
                    }
                }
            }

        });
    }
}

void ChatDialog::on_sendBtn_clicked(){//点击发送键发送消息
    if(ui->inputEdit->text()!="")
    {
        tcpSocket->abort();
        tcpSocket->connectToHost(hostip, hostport);

        if(!tcpSocket->waitForConnected(3000))
        {
            QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
            this->close();
            myuser.is_online = false;
            LoginDialog *dialog = new LoginDialog();
            dialog->show();
        }
        else
        {//服务器连接成功
            QString nowstr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            //年-月-日 时：分：秒
            QString message = QString("chat_send--%1--%2--%3--%4").arg(nowstr).arg(myuser.name).arg(other_user.name).arg(ui->inputEdit->text());
            tcpSocket->write(message.toUtf8());
            tcpSocket->flush();
            ui->inputEdit->clear();
            connect(tcpSocket,
                    &QTcpSocket::readyRead,
                    [=](){
                QString buffer = QString(tcpSocket->readAll());
                if(QString(buffer).section("--",0,0) == QString("chat_send_error")){
                    QMessageBox::warning(this,"Warning!", "发送失败",QMessageBox::Yes);
                }
            });
        }
    }
    else
    {//空消息
        QMessageBox::warning(this, "Warning!", "不能发送空消息", QMessageBox::Yes);
    }

}

void ChatDialog::on_txtBtn_clicked(){//点击跳转至收发文件界面
    FileDialog *file_dialog = new FileDialog();
    file_dialog->show();
}

void ChatDialog::on_voice_Btn_clicked(){//点击跳转至语音通话界面
    audioChat *voice = new audioChat();
    voice->show();
}

void ChatDialog::on_friend_message_Btn_clicked(){//点击跳转至好友信息界面
    this->close();
    FriendMessageDialog * friend_message = new FriendMessageDialog();
    friend_message->show();

}

void ChatDialog::on_back_btn_clicked(){//点击返回键关闭聊天界面
    this->close();
//    HomeDialog *home = new HomeDialog();
//    home->show();
}


