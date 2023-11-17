#include "groupchatdialog.h"
#include "ui_groupchatdialog.h"
#include <QMessageBox>
#include <QJsonDocument>
#include<QJsonArray>
#include <QJsonObject>
#include "user.h"
#include"homedialog.h"
#include <QDateTime>

extern QString group_name;
extern QString hostip;
extern int hostport;
extern user myuser;

GroupchatDialog::GroupchatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GroupchatDialog)
{
    ui->setupUi(this);

    //设置窗口大小
    this->setFixedSize(508,720);

    //聊天框位置
    ui->listWidget->setGeometry(0,40,508,608);
    ui->listWidget->setStyleSheet("QListWidget{background-image:url(/mnt/hgfs/SharingFileVM/theme/star/chat2.jpg);}");

    //输入框位置
    ui->inputEdit->setGeometry(70,660,370,30);

    //发送键
    ui->sendBtn->setGeometry(450,660,30,30);
    ui->sendBtn->setStyleSheet("QPushButton{background-image:url(/mnt/hgfs/SharingFileVM/theme/star/chat.send.jpg);}");

    //返回键
    ui->back_Btn->setGeometry(10,5,150,30);
    ui->back_Btn->setText("back");

    //显示群聊名
    ui->label->setGeometry(194,5,120,30);
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label->setText(group_name);


    tcpSocket = new QTcpSocket();

    //进入聊天页面时就获取所有聊天记录
    getchathistory(QString("all"));

    //每隔1秒刷新
    timer = new QTimer();
    timer->start(1000);
    connect(timer,&QTimer::timeout,[=](){
        getchathistory(QString("new"));
    });
}

GroupchatDialog::~GroupchatDialog()
{
    delete ui;
}

void GroupchatDialog::getchathistory(QString judge){//获取聊天记录
    tcpSocket->abort();
    tcpSocket->connectToHost(hostip, hostport);

    if(!tcpSocket->waitForConnected(500))
    {
        QMessageBox::warning(this, "Warning!", "get_chat_history_error", QMessageBox::Yes);
    }
    else{
        QString message = QString("group_history--%1--%2").arg(group_name).arg(judge);
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

                            QString group_message = dataObject.value("text").toString();
                            QString message_time = dataObject.value("time").toString();
                            QString message_name = dataObject.value("sender").toString();
                            QString flag = QString::number(dataObject.value("flag").toInt());

                            QListWidgetItem *item_message = new QListWidgetItem(group_message);
                            QListWidgetItem *item_name = new QListWidgetItem(message_name+"  "+message_time);
                            QListWidgetItem *item_empty = new QListWidgetItem("");

                            QSize itemSize = item_message->sizeHint();
                            itemSize.setHeight(40);
                            item_message->setSizeHint(itemSize);

                            if(message_name==myuser.name){
                                item_message->setBackground(Qt::lightGray);

                            }
                            else{
                                QBrush brush(QColor(122,197,205));
                                item_message->setBackground(brush);
                            }
                            item_message->setIcon(QIcon(QString("/mnt/hgfs/SharingFileVM/pic/%1.png").arg(flag)));
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

void GroupchatDialog::on_back_Btn_clicked(){//点击关闭群聊界面
    this->close();
//    HomeDialog *home = new HomeDialog();
//    home->show();
}

void GroupchatDialog::on_sendBtn_clicked(){//点击发送按键发送消息
    if(ui->inputEdit->text()!="")
    {
        tcpSocket->abort();
        tcpSocket->connectToHost(hostip, hostport);

        if(!tcpSocket->waitForConnected(3000))
        {
            QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
        }
        else
        {//服务器连接成功
            QString nowstr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
            //年-月-日 时：分：秒
            QString message = QString("group_send--%1--%2--%3--%4").arg(nowstr).arg(group_name).arg(myuser.name).arg(ui->inputEdit->text());
            tcpSocket->write(message.toUtf8());
            tcpSocket->flush();
            ui->inputEdit->clear();
        }
    }
    else
    {//空消息
        QMessageBox::warning(this, "Warning!", "不能发送空消息", QMessageBox::Yes);
    }
}
