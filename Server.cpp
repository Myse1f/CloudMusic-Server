#include "Server.h"

Server::Server(QObject *parent) : QTcpServer(parent) {
}

void Server::incomingConnection(qintptr socketDescriptor) {
    UserThread *thread = new UserThread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}