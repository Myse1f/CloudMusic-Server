#ifndef _SERVER_H_
#define _SERVER_H_

#include <QtCore>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <string>
#include "UserThread.h"

class Server : public QTcpServer {
    Q_OBJECT
//private:
    
public:
    Server(QObject *parent = 0);
	void run();

private:
    QMap<qintptr, UserThread*> threadMap;
    // QVector<int>& getOnlineUsers();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

public slots:
    void fowardMsg(qintptr, std::string, std::string);
};

#endif
