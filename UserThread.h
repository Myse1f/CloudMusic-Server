#ifndef _USERTHREAD_H_
#define _USERTHREAD_H_

#include <QtCore>
#include <QThread>
#include <QtNetwork/QTcpSocket>
#include "Dao.h"
#include "protocol.pb.h"

class UserThread : public QThread {
    Q_OBJECT

public:    
    static QMap<int,qintptr> sockets;
    static QVector<int> onlineUsers;

private:
    int socketDescriptor;
    int userId;
    Dao database;
    QTcpSocket tcpSocket;
    Datapackage dp;
    QDataStream in;
    QString data;

public:
    UserThread(int socketDescriptor, QObject *parent);
    void run() override;
    bool isLogin();

signals:
    void error(QTcpSocket::SocketError socketError);
    void readPackage();

private slots:
    void readData();
    void handlePackage();
    void onDisconnected();
    // void terminateThread();
};

#endif