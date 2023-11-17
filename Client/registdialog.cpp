#include "registdialog.h"
#include "ui_registdialog.h"

extern QString hostip;
extern int hostport;

RegistDialog::RegistDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegistDialog)
{
    ui->setupUi(this);

    //设置背景
    ui->back_label->setGeometry(0,0,986,664);
    QImage image("/mnt/hgfs/SharingFileVM/theme/star/login1.jpg");
    ui->back_label->setPixmap(QPixmap::fromImage(image));
    ui->back_label->lower();

    //设置颜色
    ui->user_label->setStyleSheet("color:white;");
    ui->psw_label->setStyleSheet("color:white;");
    ui->check_psw_label->setStyleSheet("color:white;");

    //设置字体
    QFont font("YouYuan",20);
    QFont font_2("YouYuan",12);

    ui->user_label->setFont(font);
    ui->psw_label->setFont(font);
    ui->check_psw_label->setFont(font);

    ui->usrEdit_2->setFont(font_2);
    ui->pwdEdit_1->setFont(font_2);
    ui->pwdEdit_2->setFont(font_2);

    //设置输入框样式
    ui->usrEdit_2->setStyleSheet("background:transparent;border-width:4;border-style:outset;color:rgb(240,230,255);border-color:lightgray;");
    ui->pwdEdit_1->setStyleSheet("background:transparent;border-width:4;border-style:outset;color:rgb(240,230,255);border-color:lightgray;");
    ui->pwdEdit_2->setStyleSheet("background:transparent;border-width:4;border-style:outset;color:rgb(240,230,255);border-color:lightgray;");

}

RegistDialog::~RegistDialog()
{
    delete ui;
}

void RegistDialog::on_okBtn_clicked(){//点击注册确认按钮

    username = ui->usrEdit_2->text().trimmed();
    password_1 = ui->pwdEdit_1->text();
    password_2 = ui->pwdEdit_2->text();

    qDebug()<<password_1<<password_2;
    
    if(username==QString("")||password_1==QString("")){
        QMessageBox::warning(this,"Warning!","用户名或密码不能为空",QMessageBox::Ok);
    }
    else{
        //判度两次密码是否相同
        if(password_1 != password_2){
            QMessageBox::warning(this,"Warning!","两次密码输入不相同",QMessageBox::Ok);
            ui->pwdEdit_1->clear();
            ui->pwdEdit_2->clear();
            ui->pwdEdit_1->setFocus();
        }
        else{
            //连接服务器
            tcpSocket = new QTcpSocket();
            tcpSocket->abort();//取消已建立的连接
            tcpSocket->connectToHost(hostip,hostport);//链接服务器

            if(!tcpSocket->waitForConnected(30000)){
                //连接失败
                QMessageBox::warning(this,"Warning!","注册：服务器连接失败",QMessageBox::Ok);
                //关闭注册界面返回登陆界面
                this->close();
                LoginDialog *dialog = new LoginDialog();
                dialog->show();
            }
            else {


                QString message = QString("Regist--%1--%2").arg(username).arg(password_1);
                //向服务端发送账号密码
                tcpSocket->write(message.toUtf8());
                tcpSocket->flush();//确保立即发送
                //信号-槽函数
                connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readData()));

            }

        }
    }
}

void RegistDialog::readData(){//读取数据
    QByteArray buff = tcpSocket->readAll();
    //获取服务器返回信息 Success/Error
    QString data = QString(buff);//返回是否成功

    if(data==QString("Error")){
        //用户名已存在
        QMessageBox::warning(this,"Warning","用户名已存在",QMessageBox::Ok);
        ui->usrEdit_2->clear();
        ui->usrEdit_2->setFocus();
    }else {
        //注册成功
        QMessageBox::information(this,"information","Registe Success!",QMessageBox::Ok);
        this->close();
        LoginDialog *dialog = new LoginDialog();
        dialog->show();
    }
}

void RegistDialog::on_backBtn_clicked(){//点击返回注册界面
    this->close();
    LoginDialog *dialog = new LoginDialog();
    dialog->show();
}
