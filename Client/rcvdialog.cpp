#include "rcvdialog.h"
#include "ui_rcvdialog.h"
#include "user.h"
#include <QDebug>

extern user myuser;
extern user other_user;
extern QString hostip;
extern int hostport;
QStringList fileList;
int rcvSize = 0;
QByteArray rcvFileContent;


RcvDialog::RcvDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RcvDialog)
{
    tcpSocket = new QTcpSocket();
    //连接服务器
    tcpSocket->connectToHost(hostip,hostport);//链接服务器
    if(!tcpSocket->waitForConnected(3000))
    {
        QMessageBox::information(this,"Error:","连接服务器失败！");
        return;
    }
    else{
        QMessageBox::information(this,"Success:","连接成功！");

    }
    ui->setupUi(this);
    QString head = QString("seeFile--%1--%2").arg(myuser.name).arg(other_user.name);
    qint64 len = tcpSocket->write(head.toUtf8());
    connect(tcpSocket,&QTcpSocket::readyRead,[=](){
        QByteArray jsonData = tcpSocket->readAll();
        QJsonDocument dataDoc = QJsonDocument::fromJson(jsonData);
        qDebug() << jsonData;

        if (!dataDoc.isNull()) {

            if (dataDoc.isArray()) {
                //用来收文件
                rcv_tcpSocket = new QTcpSocket();

                QJsonArray dataArray = dataDoc.array();
                foreach (const QJsonValue &dataValue, dataArray) {
                    int current_row = 0;
                    if (dataValue.isObject()) {
                        QJsonObject dataObject = dataValue.toObject();
                        QString fileName = dataObject.value("fileName").toString();
                        int fileSize = dataObject.value("fileSize").toInt();
                        QString sendTime = dataObject.value("sendTime").toString();
                        ui->tableWidget->insertRow(current_row);
                        ui->tableWidget->setItem(current_row,0,new QTableWidgetItem(fileName));
                        ui->tableWidget->setItem(current_row,1,new QTableWidgetItem(QString::number(fileSize)));
                        ui->tableWidget->setItem(current_row,2,new QTableWidgetItem(sendTime));
                        current_row++;
                        // 在这里处理获得的数据
                        qDebug() << "File Name: " << fileName;
                        qDebug() << "File Size: " << fileSize;
                        qDebug() << "Send Time: " << sendTime;
                    }
                }
                ui->tableWidget->resizeColumnsToContents();
                connect(ui->tableWidget, &QTableWidget::itemClicked,[=](){

                    int row = ui->tableWidget->currentItem()->row();
                    int column = ui->tableWidget->currentItem()->column();
                    if (column == 0) {
                        QString text = ui->tableWidget->currentItem()->text();
                        QString sendTime = ui->tableWidget->item(row,2)->text();
                        int fileSize = ui->tableWidget->item(row,1)->text().toInt();
                        //弹出是否接收
                        QMessageBox::StandardButton box;
                        box = QMessageBox::question(this, "提示", QString("确实要接收%1吗?").arg(text), QMessageBox::Yes|QMessageBox::No);
                        if(box==QMessageBox::No)
                            return;
                        else{

                            ui->tableWidget->removeRow(row);

                            //向服务端申请下载这个文件
                            //downloadFile--text--reciver--sender
                            QString head = QString("downloadFile--%1--%2--%3--%4").arg(text).arg(myuser.name).arg(other_user.name).arg(sendTime);
                            rcv_tcpSocket->connectToHost(hostip,hostport);
                            rcv_tcpSocket->write(head.toUtf8());
                            connect(rcv_tcpSocket,
                                    &QTcpSocket::readyRead,
                                    [=](){
                                QByteArray fileContent = rcv_tcpSocket->readAll();
                                rcvFileContent += fileContent;
                                rcvSize += fileContent.size();
                                if(rcvSize == fileSize){
                                    QString fileName = QFileDialog::getExistingDirectory(this,tr("Save File"));
                                    qDebug() << fileName;
                                    QFile file(fileName+"/"+text);
                                    if(file.open(QIODevice::WriteOnly)){
                                        file.write(rcvFileContent);
                                        file.close();
                                    }
                                    rcvSize = 0;
                                    rcvFileContent.clear();
                                    QMessageBox::information(this,"information","成功存入当前设备!",QMessageBox::Ok);
                                }
                            });
                        }
                   }
                });
                rcv_tcpSocket->disconnectFromHost();
                rcv_tcpSocket->close();
            }
        }
        else{
            QMessageBox::information(this,"","没有待接收的文件！");

        }

    });

}

RcvDialog::~RcvDialog()
{
    delete ui;
}
