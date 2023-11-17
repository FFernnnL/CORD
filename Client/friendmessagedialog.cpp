#include "friendmessagedialog.h"
#include "ui_friendmessagedialog.h"
#include"user.h"
#include<QMessageBox>
#include"homedialog.h"
# include"chatdialog.h"


extern user myuser;
extern QString hostip;
extern int hostport;
extern user other_user;
extern QString name;

FriendMessageDialog::FriendMessageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FriendMessageDialog)
{
    ui->setupUi(this);

    ui->friend_name_Label->setAlignment(Qt::AlignCenter);
    ui->friend_name_Label->setStyleSheet("color:white;");
    QFont font("YouYuan",15);
    ui->friend_name_Label->setFont(font);

    create_friend_Dialog();
}

FriendMessageDialog::~FriendMessageDialog()
{
    delete ui;
}


void FriendMessageDialog::create_friend_Dialog(){//设置界面UI、获取好友昵称并显示
    //设置背景图片
    this->setFixedSize(400,600);
    ui->back_label->setGeometry(0,0,400,600);
    QImage image("/mnt/hgfs/SharingFileVM/theme/star/friend.jpg");
    ui->back_label->setPixmap(QPixmap::fromImage(image));
    ui->back_label->lower();

    //设置名字的文字颜色
    ui->friend_name_Label->setStyleSheet("color:white;");

    //显示好友头像
    ui->headBtn->setIcon(QIcon(QString("/mnt/hgfs/SharingFileVM/pic/%1.png").arg(other_user.flag_head)));
    ui->headBtn->setIconSize(QSize(ui->headBtn->width(),ui->headBtn->height()));

    tcpSocket = new QTcpSocket();
    tcpSocket->abort();
    tcpSocket->connectToHost(hostip,hostport);
    if(!tcpSocket->waitForConnected())
    {
        QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Ok);
        this->close();
        //返回到 home 页面
        HomeDialog *home = new HomeDialog();
        home->show();
    }
    else {
        //整合向服务端发送的信息
        QString message = QString("Friend_message--%1--%2").arg(myuser.name).arg(other_user.name);
        tcpSocket->write(message.toUtf8());
        tcpSocket->flush();//立即发送
        connect(tcpSocket,&QTcpSocket::readyRead,[=](){
            QString buff = QString(tcpSocket->readAll());
            QString note_name = buff.section("--",0,0);
            ui->friend_name_Label->setText(name);
        });
    }

}

void FriendMessageDialog::on_add_friend_note_Btn_clicked(){//添加备注

    tcpSocket = new QTcpSocket();
    tcpSocket->abort();
    tcpSocket->connectToHost(hostip,hostport);
    if(!tcpSocket->waitForConnected())
    {
        QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Ok);

    }
    else {
        QString note_name = ui->noteEdit->text();
        if(note_name==""){
            QMessageBox::warning(this, "Warning!", "输入不能为空", QMessageBox::Ok);
            ui->noteEdit->clear();
            ui->noteEdit->setFocus();
        }
        else{
            QString message = QString("Change_note--%1--%2--%3")
                    .arg(myuser.name).arg(other_user.name).arg(note_name);
            tcpSocket->write(message.toUtf8());
            tcpSocket->flush();
            connect(tcpSocket,&QTcpSocket::readyRead,[=](){
                QString buff = QString(tcpSocket->readAll());
                if(buff==QString("Success")){
                    QMessageBox::information(this,"提示","备注添加成功",QMessageBox::Ok);
                    other_user.note_name= note_name;
                    name = other_user.name+"--"+note_name;
                    ui->friend_name_Label->setText(QString("%1--%2").arg(other_user.name).arg(note_name));
                }
                else{
                    QMessageBox::warning(this, "Warning!", "修改失败", QMessageBox::Ok);
                    ui->noteEdit->clear();
                    ui->noteEdit->setFocus();
                }
            });
        }
    }
}

void FriendMessageDialog::on_delete_friend_Btn_clicked(){//删除好友
    int choice = QMessageBox::question(this,"提示","确认要删除吗",QMessageBox::No | QMessageBox::Yes);
    if(choice==QMessageBox::Yes){
        tcpSocket = new QTcpSocket();
        tcpSocket->abort();
        tcpSocket->connectToHost(hostip,hostport);
        if(!tcpSocket->waitForConnected())
        {
            QMessageBox::warning(this, "Warning!", "网络错误", QMessageBox::Ok);

        }
        else {
           QString message = QString("Delete_friend--%1--%2").arg(myuser.name).arg(other_user.name);
           tcpSocket->write(message.toUtf8());
           connect(tcpSocket,&QTcpSocket::readyRead,[=](){
               QString data = QString(tcpSocket->readAll());
               if(data == QString("Success")){
                   QMessageBox::information(this,"提示","删除成功",QMessageBox::Ok);
                   //删除后跳转到 home 界面
                   this->close();
                   HomeDialog *home = new HomeDialog();
                   home->show();
               }
               else{
                   QMessageBox::information(this,"提示","Delete Error",QMessageBox::Ok);
               }
           });
        }
    }
}

void FriendMessageDialog::on_pushButton_clicked(){//返回聊天界面
    this->close();
    ChatDialog *chat = new ChatDialog();
    chat->show();
}
