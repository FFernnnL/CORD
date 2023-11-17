#include "homedialog.h"
#include "ui_homedialog.h"
#include "userdialog.h"
#include "chatdialog.h"
#include <QMessageBox>
#include "logindialog.h"
#include "user.h"
#include <QDebug>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMovie>
#include <QBrush>
#include "groupchatdialog.h"
#include "aiwidget.h"

extern QString hostip;
extern int hostport;
extern user myuser;
QString name;
user other_user;
QString group_name;
QList <QString> flag_head_list;
QList <QString> ip_list;
QString new_num;

HomeDialog::HomeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HomeDialog)
{
    ui->setupUi(this);
    this->setFixedSize(700,680);

    //设置显示用户名
    ui->namelabel->setAlignment(Qt::AlignCenter);
    ui->namelabel->setText(myuser.name);

    //设置主界面的背景图片
    ui->back_label->setGeometry(0,0,1000,680);
    QImage image("/mnt/hgfs/SharingFileVM/theme/star/home1.jpg");
    ui->back_label->setPixmap(QPixmap::fromImage(image));
    ui->back_label->lower();

    //将label背景设置为透明，与底图融为一体
    ui->gif_label->setStyleSheet("background-color:transparent;");
    ui->namelabel->setStyleSheet("background-color:transparent;");
    ui->label->setStyleSheet("background-color:transparent;");

    //设置文字颜色
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::white);
    ui->namelabel->setPalette(pe);
    ui->label->setPalette(pe);

    //设置字体和大小
    QFont font("YouYuan",15);
    ui->label->setFont(font);
    ui->namelabel->setFont(font);

    //将好友列表和群聊列表设置为透明、灰色框
    ui->friendlistWidget->setStyleSheet("background-color:transparent;border:3px solid lightgray;");
    ui->groupchat_list->setStyleSheet("background-color:transparent;border:3px solid lightgray;");

    //添加动图
    ui->gif_label->setGeometry(268,467,135,160);
    QImage image_gif(QString("/mnt/hgfs/SharingFileVM/theme/gif/5.gif"));
    ui->gif_label->setPixmap(QPixmap::fromImage(image_gif));
    QMovie *movie = new QMovie(QString("/mnt/hgfs/SharingFileVM/theme/gif/5.gif"));
    ui->gif_label->setMovie(movie);
    movie->start();

    //设置跳转智能聊天机器人键的图标
    ui->robot_Btn->setIcon(QIcon("/mnt/hgfs/SharingFileVM/theme/star/robot2.jpg"));
    ui->robot_Btn->setIconSize(QSize(ui->robot_Btn->width(),ui->robot_Btn->height()));

    //initial
    create_friendlist();
    creat_groupchat_list();
    get_PicMessage_2();

}

HomeDialog::~HomeDialog()
{
    delete ui;
    tcpSocket->close();
}

void HomeDialog::on_searchBtn_clicked(){//点击搜索并添加好友
    QString addfriend_name = ui->searchEdit->text();
    if(!addfriend_name.isEmpty()){
        if(addfriend_name != myuser.name){
            QTcpSocket *temp_socket = new QTcpSocket();
            temp_socket = new QTcpSocket();
            temp_socket->abort();//取消已有链接
            temp_socket->connectToHost(hostip, hostport);//链接服务器
            if(!temp_socket->waitForConnected(3000))
            {
                QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
            }
            else{
                QString message = QString("friend_add--%1--%2").arg(myuser.name).arg(addfriend_name);
                temp_socket->write(message.toUtf8());
                temp_socket->flush();

                connect(temp_socket,
                        &QTcpSocket::readyRead,
                        [=](){
                    QByteArray buffer = temp_socket->readAll();
                    if(QString(buffer) == QString("add_friend_success")){
                        QMessageBox::information(this, "Message", "添加成功！" ,QMessageBox::Yes);
                        ui->searchEdit->clear();
                        ui->searchEdit->setFocus();
                        //refresh the friend-List
                        HomeDialog::create_friendlist();
                    }
                    else if(QString(buffer) == QString("add_friend_fail")){
                        QMessageBox::warning(this, "Warning", "此用户不存在！", QMessageBox::Yes);
                    }
                    else if(QString(buffer)=="friend_exist"){
                        QMessageBox::warning(this, "Warning", "Friend Exist!", QMessageBox::Yes);
                    }
                });
            }

        }
        else{
            QMessageBox::warning(this, "Warning", "不能添加自己为好友！", QMessageBox::Yes);
        }
    }

}

void HomeDialog::on_search_group_Btn_clicked(){//点击搜索并加入群聊
    QString addgroup_name = ui->search_group_Edit->text();
    if(!addgroup_name.isEmpty()){
            QTcpSocket *temp_socket = new QTcpSocket();
            temp_socket = new QTcpSocket();
            temp_socket->abort();//取消已有链接
            temp_socket->connectToHost(hostip, hostport);//链接服务器
            if(!temp_socket->waitForConnected(3000))
            {
                QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
            }
            else{
                QString message = QString("group_add--%1--%2").arg(addgroup_name).arg(myuser.name);
                temp_socket->write(message.toUtf8());
                temp_socket->flush();

                connect(temp_socket,&QTcpSocket::readyRead,[=](){
                    QByteArray buffer = temp_socket->readAll();

                    if(QString(buffer) == QString("Success")){
                        QMessageBox::information(this, "Message", "添加成功！" ,QMessageBox::Yes);
                        //refresh the friend-List
                        HomeDialog::creat_groupchat_list();
                        ui->search_group_Edit->clear();
                        ui->search_group_Edit->setFocus();
                    }
                    else if(QString(buffer) == QString("group_exist")){
                        QMessageBox::warning(this, "Warning", "group exist！", QMessageBox::Yes);
                    }
                    else if(QString(buffer) == QString("Error")){
                        QMessageBox::warning(this, "Warning", "group not exist！", QMessageBox::Yes);
                    }
                });

             }
    }
    else{
        QMessageBox::warning(this,"warning!","Can't input empty",QMessageBox::Ok);
    }
}

void HomeDialog::on_creat_group_Btn_clicked(){//点击创建群聊
    QString group_message = ui->create_group_Edit->text();
    if(!group_message.isEmpty()){
        if(group_message != myuser.name){
            QTcpSocket *temp_socket = new QTcpSocket();
            temp_socket = new QTcpSocket();
            temp_socket->abort();
            temp_socket->connectToHost(hostip, hostport);
            if(!temp_socket->waitForConnected(3000))
            {
                QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Yes);
            }
            else{
                QString message = QString("create_group--%1--%2").arg(group_message).arg(myuser.name);
                temp_socket->write(message.toUtf8());
                temp_socket->flush();

                connect(temp_socket,&QTcpSocket::readyRead,[=](){
                    QByteArray buffer = temp_socket->readAll();

                    if(QString(buffer) == QString("Success")){
                        QMessageBox::information(this, "Message", "Create Group Success！" ,QMessageBox::Yes);
                        ui->searchEdit->clear();
                        ui->searchEdit->setFocus();

                        //refresh the friend-List
                        HomeDialog::creat_groupchat_list();
                    }
                    else if(QString(buffer)=="group_exists"){
                        QMessageBox::warning(this, "Warning", "group_exists", QMessageBox::Yes);
                    }
                });
            }
        }
    }
}

void HomeDialog::create_friendlist(){//创建好友列表
    ui->friendlistWidget->clear();
    QTcpSocket *temp_socket = new QTcpSocket();
    temp_socket->abort();//取消已有链接
    temp_socket->connectToHost(hostip, hostport);//链接服务器
    if(!temp_socket->waitForConnected(3000))
    {

        QMessageBox::warning(this, "Warning!", "create_friend_error", QMessageBox::Yes);

    }
    else{
        QString message = QString("get_friendlist--%1").arg(myuser.name);
        temp_socket->write(message.toUtf8());
        temp_socket->flush();
        connect(temp_socket,&QTcpSocket::readyRead,[=](){
            //QMessageBox::information(this,"infor","Hello!",QMessageBox::Yes);
            QByteArray jsondata = temp_socket->readAll();
            QJsonDocument dataDoc = QJsonDocument::fromJson(jsondata);

            if(!dataDoc.isNull()){
                if(dataDoc.isArray()){
                    QJsonArray dataArray = dataDoc.array();
                    foreach (const QJsonValue &dataValue, dataArray) {
                        if(dataValue.isObject()){
                            QJsonObject dataObject = dataValue.toObject();
                            QString friend_name = dataObject.value("friend").toString();
                            QString ip = dataObject.value("last_ip").toString();
                            QString flag_head = QString::number(dataObject.value("flag").toInt());
                            QString note_name = dataObject.value("remark").toString();
                            QString name_ = (friend_name==note_name) ? friend_name:friend_name+"--"+note_name;
                            flag_head_list.append(flag_head);
                            ip_list.append(ip);
                            QListWidgetItem *item = new QListWidgetItem(name_);
                            item->setIcon(QIcon(QString("/mnt/hgfs/SharingFileVM/pic/%1.png").arg(flag_head)));
                            item->setBackground(Qt::lightGray);

                            QSize itemSize = item->sizeHint();
                            itemSize.setHeight(40);
                            item->setSizeHint(itemSize);
                            ui->friendlistWidget->addItem(item);
                        }
                    }
                }
            }

        });
    }
}

void HomeDialog::creat_groupchat_list(){//创建群聊列表
    ui->groupchat_list->clear();
    QTcpSocket *temp_socket = new QTcpSocket();
    temp_socket->abort();
    temp_socket->connectToHost(hostip, hostport);
    if(!temp_socket->waitForConnected(3000))
    {
        QMessageBox::warning(this, "Warning!", "create group chat error", QMessageBox::Yes);
    }
    else{
        QString message = QString("get_grouplist--%1").arg(myuser.name);
        temp_socket->write(message.toUtf8());
        temp_socket->flush();
        connect(temp_socket,&QTcpSocket::readyRead,[=](){
            QByteArray jsondata = temp_socket->readAll();
            QJsonDocument dataDoc = QJsonDocument::fromJson(jsondata);

            if(!dataDoc.isNull()){
                if(dataDoc.isArray()){
                    QJsonArray dataArray = dataDoc.array();
                    foreach (const QJsonValue &dataValue, dataArray) {
                        if(dataValue.isObject()){
                            QJsonObject dataObject = dataValue.toObject();

                            QString group_name = dataObject.value("group").toString();
                            QListWidgetItem *item = new QListWidgetItem(group_name);
                            //set the color
                            item->setBackground(Qt::lightGray);
                            //set the height
                            QSize itemSize = item->sizeHint();
                            itemSize.setHeight(40);
                            item->setSizeHint(itemSize);
                            //add to list
                            ui->groupchat_list->addItem(item);
                        }
                    }
                }
            }

        });
    }
}

void HomeDialog::on_friendlistWidget_itemDoubleClicked(){//双击好友，跳转到聊天界面
    other_user.name = (ui->friendlistWidget->currentItem()->text()).section("--",0,0);
    other_user.flag_head = flag_head_list.at(ui->friendlistWidget->currentRow());
    other_user.ip = ip_list.at(ui->friendlistWidget->currentRow());
    name = ui->friendlistWidget->currentItem()->text();
    ChatDialog *cht = new ChatDialog();
    cht->show();
}

void HomeDialog::on_groupchat_list_itemDoubleClicked(){//双击群聊，跳转到群聊界面
    group_name = ui->groupchat_list->currentItem()->text();
    GroupchatDialog *groupchat = new GroupchatDialog();
    groupchat->show();
}

void HomeDialog::get_PicMessage_2(){//获取头像信息
    QTcpSocket *temp_socket = new QTcpSocket();
    temp_socket->abort();
    temp_socket->connectToHost(hostip, hostport);
    if(!temp_socket->waitForConnected(30000))
    {
        QMessageBox::warning(this, "Warning!", "get_head_error", QMessageBox::Yes);
        this->close();
        LoginDialog *log = new LoginDialog();
        log->show();
    }
    else{
        QString message = QString("get_flag--%1").arg(myuser.name);
        temp_socket->write(message.toUtf8());
        temp_socket->flush();
        connect(temp_socket,&QTcpSocket::readyRead,[=](){
            QByteArray buffer = temp_socket->readAll();

            new_num = QString(buffer);
            if(new_num=="get_flag_error")
            {
                QMessageBox::warning(this, "Warning!", "获取失败！", QMessageBox::Yes);
            }
            else{
                myuser.flag_head= new_num;
                changeHomeBtn();
            }
        });
    }
}

void HomeDialog::changeHomeBtn(){//更换主界面头像
    ui->ownBtn->setIcon(QIcon(QString("/mnt/hgfs/SharingFileVM/pic/%1.png").arg(new_num)));
    ui->ownBtn->setIconSize(QSize(ui->ownBtn->width(),ui->ownBtn->height()));
}

void HomeDialog::on_freshBtn_clicked(){//点击刷新主界面
    this->close();
    HomeDialog *home = new HomeDialog();
    home->show();
}

void HomeDialog::on_ownBtn_clicked(){//点击跳转个人信息界面
    this->close();
    UserDialog *u = new UserDialog();
    u->show();
}

void HomeDialog::on_robot_Btn_clicked(){//点击跳转智能聊天机器人界面
    AIWidget *ai = new AIWidget();
    ai->show();
}
