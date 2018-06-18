#include "Server.h"
#include <QtDebug>
//#include <iostream>

#define PORT 10000

Server::Server(QObject *parent) : QTcpServer(parent) {
	qDebug() << "Server";
}

void Server::incomingConnection(qintptr socketDescriptor) {
	qDebug() << "new connection";
    UserThread *thread = new UserThread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void Server::run() {
	this->listen(QHostAddress::Any, PORT);
	//exec();
}
