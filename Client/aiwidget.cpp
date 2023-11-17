#include "aiwidget.h"
#include "ui_aiwidget.h"
#include "aichat.h"
#include "user.h"

extern user myuser;

AIWidget::AIWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AIWidget)
{
    ui->setupUi(this);

    this->setFixedSize(508,720);

    //聊天框
    ui->listWidget->setGeometry(0,40,508,608);
    ui->listWidget->setStyleSheet("QListWidget{background-image:url(/mnt/hgfs/SharingFileVM/theme/star/chat2.jpg);}");

    //输入框
    ui->inputEdit->setGeometry(20,660,420,30);

    //发送键
    ui->sendBtn->setGeometry(450,660,50,30);

    //显示名字Clan
    ui->label->setGeometry(210,5,120,30);
    ui->label->setText(QString("Clan"));


    QString nowstr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QListWidgetItem *item_name = new QListWidgetItem("Clan  "+nowstr);
    QListWidgetItem *item_message = new QListWidgetItem("您好，欢迎使用，我是聊天机器人Clan，很高兴和您对话。");
    QSize itemSize = item_message->sizeHint();
    itemSize.setHeight(40); // 设置新的高度，保持宽度不变
    item_message->setSizeHint(itemSize);
    QBrush brush(QColor(122,197,205));
    item_message->setBackground(brush);
    item_message->setIcon(QIcon("/mnt/hgfs/SharingFileVM/robot.jpg"));
    ui->listWidget->addItem(item_name);
    ui->listWidget->addItem(item_message);
    ui->listWidget->addItem(new QListWidgetItem(""));
}

AIWidget::~AIWidget()
{
    delete ui;
}

void AIWidget::on_sendBtn_clicked(){//给聊天机器人发送消息
    if(ui->inputEdit->text()!="")
    {
        QString nowstr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        //年-月-日 时：分：秒
        QString text = ui->inputEdit->text();
        ui->inputEdit->clear();
        QListWidgetItem *item_message = new QListWidgetItem(text);

        //记得改成用户名
        QListWidgetItem *item_name = new QListWidgetItem(myuser.name+"  "+nowstr);
        QSize itemSize = item_message->sizeHint();
        itemSize.setHeight(40); // 设置新的高度，保持宽度不变
        item_message->setSizeHint(itemSize);
        item_message->setBackground(Qt::lightGray);
        item_message->setIcon(QIcon(QString("/mnt/hgfs/SharingFileVM/pic/%1.png").arg(myuser.flag_head)));

        ui->listWidget->addItem(item_name);
        ui->listWidget->addItem(item_message);
        ui->listWidget->addItem(new QListWidgetItem(""));


        item_name = new QListWidgetItem(QString("Clan")+"  "+nowstr);
        item_message = new QListWidgetItem(aichat(text));
        QSize new_itemSize = item_message->sizeHint();
        new_itemSize.setHeight(40); // 设置新的高度，保持宽度不变
        item_message->setSizeHint(new_itemSize);
        QBrush new_brush(QColor(122,197,205));
        item_message->setBackground(new_brush);
        item_message->setIcon(QIcon("/mnt/hgfs/SharingFileVM/robot.jpg"));

        ui->listWidget->addItem(item_name);
        ui->listWidget->addItem(item_message);
        ui->listWidget->addItem(new QListWidgetItem(""));

    }
    else
    {//空消息
        QMessageBox::warning(this, "Warning!", "不能发送空消息", QMessageBox::Yes);
    }


}
