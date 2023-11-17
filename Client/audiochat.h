#ifndef AUDIOCHAT_H
#define AUDIOCHAT_H

#include <QMainWindow>
#include <QObject>
#include <QAudioFormat>
#include <QByteArray>
#include <QHostAddress>
#include <QDebug>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>


#define MAX_AUDIO_LEN 5292000
#define BYTE_OF_60ms 5292

QT_BEGIN_NAMESPACE
namespace Ui { class audioChat; }
QT_END_NAMESPACE

class audioChat : public QMainWindow
{
    Q_OBJECT

public:
    audioChat(QWidget *parent = nullptr);

    struct video{
        int lens;
        char data[5292];
    };
    ~audioChat();

private slots:
    void on_closeBtn_clicked();
    void on_setBtn_clicked();
    void onReadyRead(bool is_rcver);
    void rcvAudio();

private:
    Ui::audioChat *ui;

    QUdpSocket *udpSocket;
    QUdpSocket *rcvCalling;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;

    QAudioOutput *output = nullptr;
    QAudioInput *input = nullptr;

    QIODevice *outputDevice = nullptr;
    QIODevice *inputDevice = nullptr;

};

#endif // AUDIOCHAT_H
