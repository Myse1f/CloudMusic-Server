#ifndef _USERTHREAD_H_
#define _USERTHREAD_H_

#include <QtCore>
#include <QThread>
#include <QtNetwork/QTcpSocket>
#include "Dao.h"
#include "protocol.pb.h"

class UserThread : public QObject {
    Q_OBJECT

public:    
    static QMap<int,qintptr> sockets;
    static QVector<int> onlineUsers;

private:
    int socketDescriptor;
    int userId;
    Dao database;
    QTcpSocket *tcpSocket;
    Datapackage dp;
    QDataStream in;
    char data[2];

public:
    UserThread(int socketDescriptor);
    //void run() override;
    bool isLogin();
    void sendMessage(std::string src, std::string text);

signals:
    void error(QTcpSocket::SocketError socketError);
    void readPackage();
    void sendMsg(qintptr, std::string, std::string);
	void quitThread();

private slots:
    void readData();
    void handlePackage();
    void onDisconnected();
    // void terminateThread();
};

#endif
