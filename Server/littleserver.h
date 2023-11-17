#ifndef LITTLESERVER_H
#define LITTLESERVER_H

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QDateTime>
#include <QSqlQuery>
#include <QListWidget>
#include <QLineEdit>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSqlRecord>
#include <QTimer>
#include <QSqlError>

namespace Ui {
class littleServer;
}

class littleServer : public QDialog
{
    Q_OBJECT

public:
    explicit littleServer(QWidget *parent = nullptr);
    ~littleServer();

private:
    Ui::littleServer *ui;
    QTcpServer* tcpServer;
    QSqlDatabase db;
    QList<QTcpSocket*> tcpSockets;
    void autoDelete();
    QHash<QString,QListWidgetItem*> m_items;
};

#endif // LITTLESERVER_H
