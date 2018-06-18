#ifndef _SERVER_H_
#define _SERVER_H_

#include <QtCore>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include "UserThread.h"

class Server : public QTcpServer {
    Q_OBJECT
private:
    //QVector<QTcpSocket*> sockets;
    //QVector<UserThread*> users;
    
public:
    Server(QObject *parent = 0);

protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

#endif
