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
	QThread *threadEntry = new QThread(this);
    UserThread *thread = new UserThread(socketDescriptor);
	threadMap.insert(socketDescriptor, thread);
	thread->moveToThread(threadEntry);
	connect(thread, SIGNAL(sendMsg(qintptr, std::string, std::string)), this, SLOT(fowardMsg(qintptr, std::string, std::string)));
    connect(thread, SIGNAL(quitThread()), threadEntry, SLOT(quit()));
	connect(threadEntry, SIGNAL(finished()), threadEntry, SLOT(deleteLater()));
    threadEntry->start();
}

void Server::run() {
	qDebug() << "Server thread id is " << QThread::currentThreadId();
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
