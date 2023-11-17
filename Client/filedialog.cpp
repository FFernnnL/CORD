#include "filedialog.h"
#include "ui_filedialog.h"
#include "rcvdialog.h"
#include "user.h"

extern user myuser;
extern user other_user;
extern QString hostip;
extern int hostport;

FileDialog::FileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileDialog)
{
    ui->setupUi(this);
//    ui->progressBar->setValue(0);
    //创建对象指定父对象
    tcpServer = new QTcpServer(this);
    connect(&timer,&QTimer::timeout,[=](){
        //关闭定时器
        timer.stop();
        //发送文件
        //connect(tcpSocket, &QTcpSocket::bytesWritten, this, &FileDialog::sendFile);
        sendFile();
    });
    //绑定监听
    tcpServer->listen(QHostAddress::Any,10100);

    //两个按钮最开始都不能按
    ui->choose_file_Btn->setEnabled(false);
    //待接收文件看看

    //连接成功后
    ui->choose_file_Btn->setEnabled(true);
    tcpSocket = new QTcpSocket();

}

FileDialog::~FileDialog()
{
    delete ui;
}

void FileDialog::sendFile(){//发送文件
    tcpSocket->write(file.readAll());
    tcpSocket->flush();
    sendSize = fileSize;
    QMessageBox::information(this,"Success:", "发送完毕");
    qDebug() << fileSize;
    ui->choose_file_Btn->setEnabled(true);
    tcpSocket->disconnectFromHost();
    tcpSocket->close();

}

void FileDialog::on_choose_file_Btn_clicked(){//点击选择文件发送
    QString filePath = QFileDialog::getOpenFileName(this, tr("选择文件"), "/home");

    if(!filePath.isEmpty()){
        fileSize = 0;
        sendSize = 0;
        //获取文件信息
        QFileInfo info(filePath);
        fileName = info.fileName();
        fileSize = info.size();
        //file关联一下
        file.setFileName(filePath);
        //以只读方式打开文件
        bool isOK = file.open(QIODevice::ReadOnly);
        if(!isOK)
        {
            QMessageBox::information(this,"Error:","只读方式打开文件失败");
            return;
        }
        else
        {
            //提示打开文件路径
            ui->root->setText("选择文件成功："+filePath);
            ui->choose_file_Btn->setEnabled(false);
        }
        //先发送文件头信息 action--文件名--文件大小--发送者--接受者--发送时间
        sender = myuser.name;
        receiver = other_user.name;
        sendTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString head = QString("sendFile--%1--%2--%3--%4--%5").arg(receiver).arg(fileName).arg(fileSize).arg(sender).arg(sendTime);

        tcpSocket->connectToHost(hostip,hostport);
        if(!tcpSocket->waitForConnected(3000))
        {
            QMessageBox::information(this,"Error:","连接服务器失败！");
            return;
        }
        else{
            QMessageBox::information(this,"Success:","连接成功！");
        }
        qint64 len = tcpSocket->write(head.toUtf8());

        //判断信息是否发送成功
        if(len > 0)
        {
            //发送文件信息，防止tcp黏包信息，定时器延时
            timer.start(20);
        }
        else
        {
            QMessageBox::information(this,"Error:","头部信息发送失败");
            file.close();
            return;
        }

    }
    else{
        QMessageBox::information(this,"Error:","选择失败，请重试！");
        return;
    }
}

void FileDialog::on_rcv_Btn_clicked(){//点击跳转到待接收文件列表
    RcvDialog *rcv = new RcvDialog();
    rcv->show();
}

void FileDialog::closeEvent(QCloseEvent *event){
    timer.stop();
}
