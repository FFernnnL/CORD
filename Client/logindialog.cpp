#include "logindialog.h"
#include "ui_logindialog.h"
#include"movie.h"
#include<QImage>
#include<QMovie>
#include <QHBoxLayout>
#include <QColor>
#include <QFont>

QString hostip = "192.168.114.14";
int hostport = 10099;
user myuser;

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{

    ui->setupUi(this);
    this->setFixedSize(986,664);

    //设置背景
    ui->back_lable->setGeometry(0,0,986,664);
    QImage image("/mnt/hgfs/SharingFileVM/theme/star/login1.jpg");
    ui->back_lable->setPixmap(QPixmap::fromImage(image));
    ui->back_lable->lower();

    //添加动图
    ui->gif_pic->setGeometry(403,467,180,180); // 401,631
    QImage image_gif(QString("/mnt/hgfs/SharingFileVM/theme/gif/boqi.gif"));
    ui->gif_pic->setPixmap(QPixmap::fromImage(image_gif));
    QMovie *movie = new QMovie(QString("/mnt/hgfs/SharingFileVM/theme/gif/boqi.gif"));
    ui->gif_pic->setMovie(movie);
    movie->start();

    //设置字体颜色和边框
    ui->label->setStyleSheet("color:white;");
    ui->label_2->setStyleSheet("color:white;");
    QFont font("YouYuan",24);
    QFont font_2("YouYuan",16);
    QFont font_3("YouYuan",14);
    ui->label->setFont(font);
    ui->label_2->setFont(font);
    ui->usrEdit->setFont(font_2);
    ui->pwdEdit->setFont(font_2);
    ui->usrEdit->setStyleSheet("background:transparent;border-width:4;border-style:outset;color:rgb(240,230,255);border-color:lightgray;");
    ui->pwdEdit->setStyleSheet("background:transparent;border-width:4;border-style:outset;border-color:lightgray;");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}


void LoginDialog::readData(){//客户端读取数据
    QString buff = QString(tcpSocket->readAll());
    //接收客户端消息判断是否成功  Success/Error--name/Error--password/Error
    QString data = buff.section("--",0,0);
    //用户是否存在
    QString data_name = buff.section("--",1,1);
    //密码是否正确
    QString data_psw = buff.section("--",2,2);
    //成功登陆
    if(data == QString("Success")){
        myuser.name = username;
        myuser.password = password;
        myuser.is_online = true;
        //关闭登陆界面，skip to movie
        this->close();
        Movie *movie = new Movie();
        movie->show();

    }
    else if(data == QString("Error")){
        //用户不存在
        if(data_name==QString("Error")){
            QMessageBox::warning(this,"Warning!","用户不存在",QMessageBox::Yes);
            ui->usrEdit->clear();
            ui->usrEdit->setFocus();
        }else{
            QMessageBox::warning(this,"Warning!","密码错误",QMessageBox::Yes);
            ui->pwdEdit->clear();
            ui->pwdEdit->setFocus();
        }

    }
}

void LoginDialog::on_loginBtn_clicked(){//点击登陆按钮
//    // skip the login
//    //调试用
//    this->close();
//    HomeDialog *home = new HomeDialog();
//    home->show();

   //获取输入的账号和密码
    username = ui->usrEdit->text().trimmed();
    password = ui->pwdEdit->text();
    //判断是否为空
    if (username == "" || password==""){
        QMessageBox::warning(this,tr("Warning"),tr("账号密码不能为空！"),QMessageBox::Yes);
    }
    else {
        //账号密码不为空，连接服务器
        tcpSocket = new QTcpSocket();
        tcpSocket->connectToHost(hostip,hostport);//连接服务器
        //等待连接成功
        if(!tcpSocket->waitForConnected()){
            QMessageBox::warning(this,"Waring!","连接服务端失败",QMessageBox::Yes);

        }else{
            //获取客户端ip / 客户信息
            QString user_message = QString("Login--%1--%2").arg(username).arg(password);
            //发送到服务端

            if(tcpSocket->write(user_message.toUtf8())==-1){
                QMessageBox::warning(this,"Waring!","write message:发送信息到服务端失败",QMessageBox::Yes);
            }else{
                //立即发送
                tcpSocket->flush();
                connect(tcpSocket,&QTcpSocket::readyRead,this,&LoginDialog::readData);
            }
        }
    }
}

void LoginDialog::on_regBtn_clicked(){//点击注册按钮
    //关闭登陆界面，打开注册界面
    this->close();
    RegistDialog *reg = new RegistDialog();
    reg->show();
}
