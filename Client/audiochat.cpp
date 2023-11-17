#include "audiochat.h"
#include "ui_audiochat.h"
#include "user.h"

extern user other_user;
int rcvPort = 10097;
int sendPort = 10098;
int rcvCallPort = 10096;
int sampleRate = 44100;
int sampleSize = 16;
int channelCount = 1;
//quint32 peer_ipv4;


audioChat::audioChat(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::audioChat)
{

    //加上this，关闭页面时自动摧毁
    ui->setupUi(this);

    ui->background->setGeometry(0,0,800,600);
    QImage image("/mnt/hgfs/SharingFileVM/theme/star/home1.jpg");
    ui->background->setPixmap(QPixmap::fromImage(image));
    ui->background->lower();

    udpSocket = new QUdpSocket(this);

    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any,rcvCallPort);

    QAudioFormat Format;
    Format.setSampleRate(sampleRate);
    Format.setSampleSize(sampleSize);
    Format.setChannelCount(channelCount);
    Format.setCodec("audio/pcm");
    Format.setSampleType(QAudioFormat::SignedInt);
    Format.setByteOrder(QAudioFormat::LittleEndian);

    output = new QAudioOutput(Format);
    input = new QAudioInput(Format);


    //接听端
    connect(tcpServer,&QTcpServer::newConnection,[=](){
        QTcpSocket* tcpSocket = tcpServer->nextPendingConnection();
        connect(tcpSocket, &QTcpSocket::readyRead,[=](){
            QByteArray buffer = tcpSocket->readAll();
            if(QString(buffer) == "calling"){
//                peer_ipv4 = tcpSocket->peerAddress().toIPv4Address();
                //弹出是否接听
                QMessageBox::StandardButton box;
                box = QMessageBox::question(this, "提示", QString("要接听来自"+other_user.name+"的语音通话吗？"), QMessageBox::Yes|QMessageBox::No);
                if(box==QMessageBox::No)
                    return;
                else{
                    tcpSocket->write(QString("ok").toUtf8());
                    qDebug()<<"对方已接听！";

                    //out
                    outputDevice = output->start();

                    //in
                    inputDevice = input->start();
                    bool bindResult = udpSocket->bind(QHostAddress::AnyIPv4,sendPort);
                    if(!bindResult){
                        QMessageBox::warning(this,tr("Warning"),
                                tr("初始化失败！"),
                                QMessageBox::Yes);
                        return;
                    }
                    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(rcvAudio()));
                    connect(inputDevice, &QIODevice::readyRead, [=]{
                        onReadyRead(true);
                    });
                    connect(tcpSocket, &QTcpSocket::disconnected, [=](){
                        //对方已挂断
                        tcpSocket->close();
                        input->stop();
                        output->stop();
                        this->close();
                    });
                }
            }


        });

    });
}

audioChat::~audioChat()
{
    delete ui;
}

void audioChat::on_setBtn_clicked(){//发起语音聊天

    qDebug()<<"向"+other_user.name+"发起语音聊天中...";
    tcpSocket = new QTcpSocket();
    tcpSocket->connectToHost(other_user.ip,rcvCallPort);
    tcpSocket->write(QString("calling").toUtf8());
    //接收对方的回复
    connect(tcpSocket, &QTcpSocket::readyRead,[=](){
        QByteArray buffer = tcpSocket->readAll();
        qDebug()<<buffer;
        if(QString(buffer) == "ok"){
            qDebug()<<"对方已接听！";
            //out
            outputDevice = output->start();

            //in
            inputDevice = input->start();
            bool bindResult = udpSocket->bind(QHostAddress::AnyIPv4,rcvPort);
            if(!bindResult){
                QMessageBox::warning(this,tr("Warning"),
                        tr("初始化失败！"),
                        QMessageBox::Yes);
                return;
            }
            connect(udpSocket,SIGNAL(readyRead()),this,SLOT(rcvAudio()));
            connect(inputDevice, &QIODevice::readyRead, [=]{
                onReadyRead(false);
            });
            connect(tcpSocket, &QTcpSocket::disconnected, [=](){
                //对方已挂断
                tcpSocket->close();
                input->stop();
                output->stop();
                this->close();
            });

        }
    });

}

void audioChat::rcvAudio(){//接收对方发来的音频
    while(udpSocket->hasPendingDatagrams()){
        qDebug()<<"audio is being received..."<<endl;
        video vp;
        memset(&vp,0,sizeof(vp));
        udpSocket->readDatagram((char*)&vp,sizeof(vp));
        outputDevice->write(vp.data,vp.lens);
    }
}

void audioChat::onReadyRead(bool is_rcver){//发送音频给对方
    video vp;
    memset(&vp.data,0,sizeof(vp));

    vp.lens = inputDevice->read(vp.data,5292);
    if(is_rcver){
        int num = udpSocket->writeDatagram((const char*)&vp, sizeof(vp),QHostAddress(other_user.ip.toStdString().c_str()),rcvPort);
//        qDebug()<<peer_ipv4;
        qDebug()<<"接听端"<<num;
    }
    else{
        int num = udpSocket->writeDatagram((const char*)&vp, sizeof(vp),QHostAddress(other_user.ip.toStdString().c_str()),sendPort);
        qDebug()<<"发起端"<<num;
    }

}

void audioChat::on_closeBtn_clicked(){//结束语音聊天
    tcpSocket->write(QString("quit").toUtf8());
    tcpSocket->flush();
    tcpSocket->close();
    input->stop();
    output->stop();
    this->close();

}
