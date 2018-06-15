#ifndef _USERTHREAD_H_
#define _USERTHREAD_H_

#include <QtCore>
#include <QThread>
#include <QtNetwork/QTcpSocket>
#include "Dao.h"
#include "protocol.pb.h"

class UserThread : public QThread {
    Q_OBJECT
private:
    int socketDescriptor;
    int userId;
    Dao database;
    QTcpSocket tcpSocket;
    Datapackage dp;
    QDataStream in;
    QString data;
    bool ready;

public:
    UserThread(int socketDescriptor, QObject *parent);
    void run() override;

signals:
    void error(QTcpSocket::SocketError socketError);

private slots:
    void readData();
};

#endif