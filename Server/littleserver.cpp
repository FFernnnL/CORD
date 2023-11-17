#include "littleserver.h"
#include "ui_littleserver.h"

int sendSize;
QStringList fileInfo;
QByteArray fileContent;
QStringList info;


littleServer::littleServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::littleServer)
{

    tcpServer = new QTcpServer();
    QTimer *timer = new QTimer(this);
    QTimer *sevenDelTimer = new QTimer(this);

    ui->setupUi(this);
    ui->lcdNumber->setDigitCount(8);
    timer->start(1000);
    connect(timer, &QTimer::timeout, [=]{
        QTime current_time = QTime::currentTime();
        ui->lcdNumber->display(current_time.toString("hh:mm:ss"));
    });
    sevenDelTimer->start(7*24*60*60*1000);
    connect(sevenDelTimer, &QTimer::timeout, this, &littleServer::autoDelete);

    ui->listWidget->clear();
    ui->listWidget->setUniformItemSizes(false);


    tcpServer->listen(QHostAddress::Any,10099);

    //数据库初始化
    /*
    用户信息表userInfo（用户名，密码，头像信息flag,（登录状态）），
    用户好友表friend_username（好友名，备注），
    待接收文件表file（文件名fileName TEXT，文件大小fileSize INTERGER，文件内容fileContent BLOB,
            发送者sender TEXT，接受者receiver TEXT，发送时间sendTime TEXT(YYYY-MM-DD HH:MM:SS)），ps:收到后从服务端删除,有待接收文件提醒。。
    消息记录表msg（发送者，接受者，内容，时间，（）），ps:服务端文件已发送提示。。。语音消息

    */
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("jinitaimei.db");
    db.open();
    QSqlQuery sqlquery;
    sqlquery.exec("create table if not exists file (\
                  receiver TEXT not NULL,\
                  fileName TEXT not NULL,\
                  fileSize TEXT not NULL,\
                  sender TEXT not NULL, \
                  sendTime TEXT,fileContent BLOB)");
    sqlquery.clear();

    sqlquery.exec("create table if not exists userInfo(\
                username TEXT primary key,\
                password TEXT not NULL,\
                theme INTEGER not NULL,\
                last_ip TEXT not NULL,\
                flag INTEGER not NULL)");

    sqlquery.clear();

    sqlquery.exec("create table if not exists groupInfo(\
                name TEXT primary key)");

    sqlquery.clear();\

    sqlquery.exec("create table if not exists msg(\
                        sender TEXT,\
                        rcver TEXT,\
                        txt TEXT,\
                        time TEXT)");
    sqlquery.exec("create table if not exists msg_group(\
                        groupname TEXT,\
                        sender TEXT,\
                        txt TEXT,\
                        time TEXT)");
    sqlquery.clear();
    db.close();


    //处理客户端的连接
    connect(tcpServer,&QTcpServer::newConnection,[=](){

        QTcpSocket* tcpSocket = tcpServer->nextPendingConnection();
        tcpSockets.append(tcpSocket);


        connect(tcpSocket, &QTcpSocket::readyRead,[=](){
            //读取缓冲区数据
            db.open();
            QSqlQuery sqlquery;
            QByteArray buffer = tcpSocket->readAll();
            QString headhead = QString(buffer).section("--",0,0);

            if("Login" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1是usrname,2时pwd
                sqlquery.clear();
                sqlquery.exec(QString("select * from userInfo where username = '%1'").arg(info[1]));
                if(!sqlquery.next())
                {//no user
                    tcpSocket->write("Error--Error--Error");
                    qDebug() << "Error--Error--Error";
                    tcpSocket->flush();
                }
                else
                {//
                    QString pwd = sqlquery.value("password").toString();
                    if(pwd == info[2]){
                        tcpSocket->write(QString("Success--%1--%2").arg(info[1],info[2]).toUtf8());
                        qDebug() << QString("Success--%1--%2").arg(info[1],info[2]);
                        tcpSocket->flush();
                        sqlquery.clear();
                        sqlquery.exec(QString("update userInfo\
                                        set last_ip= '%1'\
                                        where name='%2'").arg(QHostAddress(tcpSocket->peerAddress().toIPv4Address()).toString(),info[1]));
                        QDateTime current_time =QDateTime::currentDateTime();
                        QString now_time =current_time.toString("yyyy.MM.dd hh:mm:ss");
                        if(m_items.contains(info[1])){
                            QListWidgetItem *item = m_items.value(info[1]);
                            item->setText(QString("%1 上线时间: %2, 用户ip: %3").arg(info[1],now_time,QHostAddress(tcpSocket->peerAddress().toIPv4Address()).toString()));

                        }
                        else{
                            QListWidgetItem *item = new QListWidgetItem();
                            item->setText(QString("%1 上线时间: %2, 用户ip: %3").arg(info[1],now_time,QHostAddress(tcpSocket->peerAddress().toIPv4Address()).toString()));
                                ui->listWidget->addItem(item);
                                m_items.insert(info[1],item);
                        }

                    }
                    else{//password error
                        tcpSocket->write(QString("Error--%1--Error").arg(info[1]).toUtf8());
                        qDebug() << QString("Error--%1--Error").arg(info[1]);
                        tcpSocket->flush();
                    }
                }
            }
            else if("Regist" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //注册,1时usrname，2是pwd
                //判重
                sqlquery.clear();
                sqlquery.prepare("select * from userInfo where username = :name");
                sqlquery.bindValue(":name",info[1]);
                sqlquery.exec();
                if(!sqlquery.next())
                {//新建
                    sqlquery.clear();
                    sqlquery.exec(QString("insert into userInfo values ('%1','%2',0,'%3',0)").arg(info[1],info[2],QHostAddress(tcpSocket->peerAddress().toIPv4Address()).toString()));

                    sqlquery.exec("create table if not exists friend_"
                                + info[1]
                                +"(name TEXT primary key,\
                                    remark TEXT)");
  //qun
                    sqlquery.exec("create table if not exists group_"
                                  + info[1]
                                  +"(name TEXT primary key)");
                    tcpSocket->write(QString("Success").toUtf8());
                    qDebug() << "Success";
                    tcpSocket->flush();
                }
                else
                {
                    tcpSocket->write(QString("Error").toUtf8());
                    qDebug() << "Error";
                }

            }
            else if("friend_add" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1是usrname，2是friendname
                sqlquery.clear();
                sqlquery.exec(QString("select * from userInfo where username = '%1'").arg(info[2]));
                if(!sqlquery.next())
                {
                    tcpSocket->write("add_friend_fail");
                    qDebug() << "add_friend_fail";
                    tcpSocket->flush();
                }
                else
                {
                    sqlquery.clear();
                    sqlquery.exec(QString("select name from friend_%1 where name='%2'").arg(info[1],info[2]));
                    if(!sqlquery.next()){
                        sqlquery.clear();
                        sqlquery.exec(QString("insert or ignore into friend_%1(name,remark) values('%2','%3')").arg(info[1],info[2],info[2]));
                        qDebug() << sqlquery.lastQuery();
                        tcpSocket->write("add_friend_success");
                        qDebug() << "add_friend_success";
                        tcpSocket->flush();
                    }
                    else{
                        tcpSocket->write("friend_exist");
                        qDebug() << "friend_exist";
                        tcpSocket->flush();
                    }
                }
            }
            else if("create_group" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //注册,1时name，2是username
                //判重
                sqlquery.clear();
                sqlquery.prepare("select * from groupInfo where name = :name");
                sqlquery.bindValue(":name",info[1]);
                sqlquery.exec();
                if(!sqlquery.next())
                {//新建
                    sqlquery.clear();
            /*gai*/ sqlquery.prepare("insert into groupInfo values (:name)");
                    sqlquery.bindValue(":name",info[1]);
                    sqlquery.exec();

                    sqlquery.exec(QString("insert into group_%1 values ('%2')").arg(info[2],info[1]));
                    tcpSocket->write(QString("Success").toUtf8());
                    qDebug() << "Success";
                    tcpSocket->flush();
                }
                else
                {
                    tcpSocket->write(QString("group_exists").toUtf8());
                    qDebug() << "Error";
                }
            }
            else if("group_add" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1是groupname，2是username
                sqlquery.clear();
                sqlquery.exec(QString("select * from groupInfo where name = '%1'").arg(info[1]));
                if(!sqlquery.next())
                {
                    tcpSocket->write("Error");
                    qDebug() << "add_group_fail";
                    tcpSocket->flush();
                }
                else
                {
                    sqlquery.clear();
                    sqlquery.exec(QString("select name from group_%1 where name='%2'").arg(info[2],info[1]));
                    if(!sqlquery.next()){
                        sqlquery.clear();
                        sqlquery.exec(QString("insert or ignore into group_%1 values('%2')").arg(info[2],info[1]));
                        qDebug() << sqlquery.lastQuery();
                        tcpSocket->write("Success");
                        qDebug() << "Success";
                        tcpSocket->flush();
                    }
                    else{
                        tcpSocket->write("group_exist");
                        qDebug() << "group_exist";
                        tcpSocket->flush();
                    }
                }
            }
            else if("Delete_friend" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1时usrname，2是friname
                sqlquery.clear();

                if(sqlquery.exec(QString("delete from friend_%1 where name = '%2'").arg(info[1],info[2])) && sqlquery.exec(QString("delete from friend_%1 where name = '%2'").arg(info[2],info[1]))){
                    tcpSocket->write(QString("Success").toUtf8());
                }
                else{
                    tcpSocket->write(QString("Fail").toUtf8());
                }

            }
            else if("delete_group" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1时groupname，2 username
                sqlquery.clear();
                sqlquery.prepare("delete from group_:u\
                                where name = :n");
                sqlquery.bindValue(":u",info[2]);
                sqlquery.bindValue(":n",info[1]);
                sqlquery.exec();
            }
            else if("Change_note" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1是username，2是friname，3是备注note
                sqlquery.clear();
                sqlquery.exec(QString("update friend_%1\
                                set remark= '%2'\
                                where name='%3'").arg(info[1],info[3],info[2]));

                tcpSocket->write("Success");
                tcpSocket->flush();

            }
            else if("chat_history" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1 user，2 friend 3 状态（all new）
                if(info[3] == "all"){
                    sqlquery.clear();
                    sqlquery.exec(QString("SELECT * FROM msg WHERE ((sender='%1' AND rcver='%2') OR (sender='%2' AND rcver='%1'))").arg(info[1],info[2]));
                }
                else{
                    sqlquery.clear();
                    sqlquery.exec(QString("SELECT * FROM msg WHERE ((sender='%1' AND rcver='%2') OR (sender='%2' AND rcver='%1')) AND (time BETWEEN datetime('now', '-1 second', 'localtime') AND datetime('now', 'localtime'))").arg(info[1],info[2]));
                }
                QJsonArray dataArray;
                while (sqlquery.next()) {
                    QJsonObject dataObject;
                    dataObject["time"] = sqlquery.value("time").toString();
                    dataObject["sender"] = sqlquery.value("sender").toString();
                    dataObject["text"] = sqlquery.value("txt").toString();
                    dataArray.append(dataObject);
                    }
                    QJsonDocument dataDoc(dataArray);
                    tcpSocket->write(dataDoc.toJson());
                    if(!dataArray.isEmpty())
                        qDebug() << dataDoc;
            }
            else if("group_history" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1 name 2 status
                if(info[2] == "all"){
                    sqlquery.clear();
                    sqlquery.exec(QString("SELECT sender,time,flag,txt FROM msg_group,userInfo WHERE msg_group.sender=userInfo.username AND msg_group.groupname='%1'").arg(info[1]));
                }
                else{
                    sqlquery.clear();
                    sqlquery.exec(QString("SELECT sender,time,flag,txt FROM msg_group,userInfo WHERE (msg_group.sender=userInfo.username) AND (time BETWEEN datetime('now', '-1 second', 'localtime') AND datetime('now', 'localtime')) AND msg_group.groupname='%1'").arg(info[1]));
                }
                QJsonArray dataArray;
                while (sqlquery.next()) {
                    QJsonObject dataObject;
                    dataObject["time"] = sqlquery.value("time").toString();
                    dataObject["sender"] = sqlquery.value("sender").toString();
                    dataObject["text"] = sqlquery.value("txt").toString();
                    dataObject["flag"] = sqlquery.value("flag").toInt();
                    dataArray.append(dataObject);
                    }
                    QJsonDocument dataDoc(dataArray);
                    tcpSocket->write(dataDoc.toJson());
                    if(!dataArray.isEmpty())
                        qDebug() << dataDoc;
            }
            else if("chat_send" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1是时间，2是sender,

                sqlquery.clear();
                sqlquery.exec(QString("insert into msg(sender,rcver,txt,time) values('%1','%2','%3','%4')").arg(info[2],info[3],info[4],info[1]));

//                tcpSocket->write("Success");
//                tcpSocket->flush();

            }
            else if("group_send" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1是时间，2是group,3 user 4 text

                sqlquery.clear();
                sqlquery.exec(QString("insert into msg_group(groupname,sender,txt,time) values('%1','%2','%3','%4')").arg(info[2],info[3],info[4],info[1]));

//                tcpSocket->write("Success");
//                tcpSocket->flush();

            }
            else if("get_friendlist" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1 user
                sqlquery.clear();
                bool a = sqlquery.exec(QString("select name,remark,flag,last_ip from userInfo,friend_%1 where userInfo.username=friend_%1.name").arg(info[1]));
                if(!a){
                    qDebug()<<sqlquery.lastError().text();
                }
                QJsonArray dataArray;
                while (sqlquery.next()) {
                    QJsonObject dataObject;
                    dataObject["friend"] = sqlquery.value("name").toString();
                    dataObject["remark"] = sqlquery.value("remark").toString();
                    dataObject["flag"] = sqlquery.value("flag").toInt();
                    dataObject["last_ip"] = sqlquery.value("last_ip").toString();
                    dataArray.append(dataObject);
                    }
                    QJsonDocument dataDoc(dataArray);
                    tcpSocket->write(dataDoc.toJson());
                    tcpSocket->flush();
                    if(!dataArray.isEmpty())
                        qDebug() << dataDoc;

            }
            else if("get_grouplist" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1 user
                sqlquery.clear();
                sqlquery.exec(QString("select * from group_%1").arg(info[1]));
                QJsonArray dataArray;
                while (sqlquery.next()) {
                    QJsonObject dataObject;
                    dataObject["group"] = sqlquery.value("name").toString();
                    dataArray.append(dataObject);
                    }
                    QJsonDocument dataDoc(dataArray);
                    tcpSocket->write(dataDoc.toJson());
                    tcpSocket->flush();
                    if(!dataArray.isEmpty())
                        qDebug() << dataDoc;

            }
            else if("Friend_message"==headhead){
                qDebug() << buffer;
                //1是username，2是friname
                info = QString(buffer).split("--");
                sqlquery.exec(QString("select flag from userInfo\
                                    where username='%1'").arg(info[2]));
                sqlquery.next();
//                sqlquery.bindValue(":f",info[2]);
//                sqlquery.exec();
                int head =  sqlquery.value("flag").toInt();

                sqlquery.clear();
                sqlquery.exec(QString("select theme from userInfo\
                                    where username='%1'").arg(info[1]));
                sqlquery.next();
//                sqlquery.bindValue(":u",info[1]);
                int sys = sqlquery.value("theme").toInt();
                qDebug()<<sys;
                sqlquery.clear();
                tcpSocket->write(QString("%1--%2--%3").arg(info[2],QString::number(head),QString::number(sys)).toUtf8());
                qDebug() << QString("%1--%2--%3").arg(info[2],QString::number(head),QString::number(sys));


            }
            else if("get_flag"==headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1 is user
                sqlquery.clear();
                sqlquery.exec(QString("select flag from userInfo where username='%1'").arg(info[1]));
                sqlquery.next();
                int flag=sqlquery.value(0).toInt();
//                qDebug() << flag;
                tcpSocket->write(QString::number(flag).toUtf8());
                qDebug() << QString::number(flag);
                tcpSocket->flush();
            }
            else if("update_flag" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                sqlquery.clear();
                sqlquery.exec(QString("update userInfo set flag = %1 where username = '%2'").arg(info[2],info[1]));
                tcpSocket->write("update_flag_success");
                qDebug() << "update_flag_success";

            }
            else if("sendFile" == headhead){
                qDebug() << buffer;
                //3是文件大小
                fileInfo = QString(buffer).split("--");
                tcpSocket->setReadBufferSize(fileInfo[3].toInt());
            }
            else if("seeFile" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                //1接收，2发送
                sqlquery.clear();
                sqlquery.exec(QString("SELECT fileName, fileSize, sendTime\
                                       FROM file \
                                       WHERE receiver = '%1' AND sender = '%2'").arg(info[1]).arg(info[2]));
                QJsonArray dataArray;
                while (sqlquery.next()) {
                       QJsonObject dataObject;
                       dataObject["fileName"] = sqlquery.value("fileName").toString();
                       dataObject["fileSize"] = sqlquery.value("fileSize").toInt();
                       dataObject["sendTime"] = sqlquery.value("sendTime").toString();
                       dataArray.append(dataObject);
                }
                QJsonDocument dataDoc(dataArray);
                tcpSocket->write(dataDoc.toJson());
                qDebug() << dataDoc;

            }
            else if("downloadFile" == headhead){
                qDebug() << buffer;
                info = QString(buffer).split("--");
                sqlquery.clear();
//                sqlquery.setForwardOnly(true);
                sqlquery.exec(QString("SELECT fileContent\
                                       FROM file \
                                       WHERE fileName = '%1' AND receiver = '%2' AND sender = '%3' AND sendTime = '%4'").arg(info[1]).arg(info[2]).arg(info[3]).arg(info[4]));
                QByteArray data;
                if (sqlquery.next()) {
                      QString dataValue= sqlquery.value("fileContent").toString();
                      data = QByteArray::fromHex(dataValue.toLatin1());
                      tcpSocket->write(data);
                      tcpSocket->flush();
                      qDebug() << data.size();
//                      sqlquery.clear();
//                      sqlquery.exec(QString("DELETE FROM file WHERE fileName = '%1' AND receiver = '%2' AND sender = '%3' AND sendTime = '%4'").arg(info[1]).arg(info[2]).arg(info[3]).arg(info[4]));

                }
            }
            else{//分好几次送过来
                qDebug()<<"fileReceiving... "<<sendSize;
                fileContent += buffer;
                sendSize += buffer.size();

                if(sendSize == fileInfo[3].toInt()){
                    sqlquery.clear();
                    sqlquery.exec(QString("insert into file values('%1','%2',%3,'%4','%5','%6')").arg(fileInfo[1],fileInfo[2],fileInfo[3],fileInfo[4],fileInfo[5],QString::fromUtf8(fileContent.toHex())));

                    qDebug() << "完成！文件大小： " << sendSize;
                    fileInfo.clear();
                    fileContent.clear();
                    sendSize = 0;
                }

            }
            db.close();
        });
        connect(tcpSocket, &QTcpSocket::disconnected, [=](){
            tcpSockets.removeOne(tcpSocket);
            tcpSocket->deleteLater();

        });
    });
}
void littleServer::autoDelete(){
    db.open();
    QSqlQuery sqlquery;
    sqlquery.exec("DELETE FROM file WHERE strftime('%s', 'now', 'localtime') - strftime('%s', sendTime) > 7 * 24 * 60 * 60");
    db.close();
}
littleServer::~littleServer()
{
    tcpServer->close();
    tcpServer->deleteLater();
    delete ui;
}
