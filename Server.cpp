#include "Server.h"
#include <QtDebug>
#include <iostream>
Server::Server(QObject *parent) : QTcpServer(parent) {
	qDebug() << "Server";
}

void Server::incomingConnection(qintptr socketDescriptor) {
	std::cout << "new connection" << endl;
    UserThread *thread = new UserThread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
