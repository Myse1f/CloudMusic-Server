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
    Datapackage dp;

public:
    UserThread(int socketDescriptor, QObject *parent);
    void run() override;

signals:
    void error(QTcpSocket::SocketError socketError);
};

#endif