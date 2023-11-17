#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QFileDialog>
#include <QTimer>
#include <QMessageBox>
#include <QLineEdit>
#include <QDateTime>
#include <QDialog>

namespace Ui {
class FileDialog;
}

class FileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileDialog(QWidget *parent = nullptr);
    ~FileDialog();

    void sendFile();//send file


protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_choose_file_Btn_clicked();


    void on_rcv_Btn_clicked();

private:
    Ui::FileDialog *ui;
    QTcpSocket *tcpSocket;
    QTcpServer *tcpServer;
    QFile file;
    QTimer timer;
    QString fileName;
    quint64 fileSize;
    quint64 sendSize;
    QString receiver;
    QString sendTime;
    QString sender;
};

#endif // FILEDIALOG_H
