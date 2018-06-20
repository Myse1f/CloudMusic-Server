#include "Server.h"
#include <QtDebug>
#include <assert.h>
//#include <iostream>

#define PORT 10000

Server::Server(QObject *parent) : QTcpServer(parent) {
	qDebug() << "Server";
}

void Server::incomingConnection(qintptr socketDescriptor) {
	qDebug() << "new connection";
    UserThread *thread = new UserThread(socketDescriptor, this);
	threadMap.insert(socketDescriptor, thread);
	connect(thread, SIGNAL(sendMsg(qintptr)), this, SLOT(fowardMsg(qintptr)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void Server::run() {
	this->listen(QHostAddress::Any, PORT);
	//exec();
}

void Server::fowardMsg(qintptr socketDescriptor, std::string src, std::string text) {
	QMap<qintptr, UserThread*>::iterator it = threadMap.find(socketDescriptor);
	assert(it != threadMap.end());
	UserThread *thread = it.value();
	thread->sendMessage(src, text);
}

// QVector<qintptr>& Server::getSockets() {
// 	return this->sockets;
// }

// QVector<int>& Server::getOnlineUsers() {
// 	return this->onlineUsers;
// }
