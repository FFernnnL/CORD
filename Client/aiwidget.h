//#ifndef AIWIDGET_H
//#define AIWIDGET_H

//#include <QDialog>

//namespace Ui {
//class AIWidget;
//}

//class AIWidget : public QDialog
//{
//    Q_OBJECT

//public:
//    explicit AIWidget(QWidget *parent = nullptr);
//    ~AIWidget();

//private:
//    Ui::AIWidget *ui;
//};

//#endif // AIWIDGET_H


#ifndef AIWIDGET_H
#define AIWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QDateTime>
QT_BEGIN_NAMESPACE
namespace Ui { class AIWidget; }
QT_END_NAMESPACE

class AIWidget : public QWidget
{
    Q_OBJECT

public:
    AIWidget(QWidget *parent = nullptr);
    ~AIWidget();

private slots:

    void on_sendBtn_clicked();


private:
    Ui::AIWidget *ui;
};
#endif // AIWIDGET_H

