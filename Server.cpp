#include "Server.h"

Server::Server(QObject *parent) : QTcpServer(parent) {
    sockets.clear();
    users.clear();
}

void Server::incomingConnection(qintptr socketDescriptor) {
    //TBD: new UserThread
}