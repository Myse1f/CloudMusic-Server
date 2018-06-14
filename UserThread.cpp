#include "UserThread.h"

UserThread::UserThread(int socketDescriptor, QObject *parent):QThread(parent), socketDescriptor(socketDescriptor) {
    userId = -1;
}

void UserThread::run() {
    QTcpSocket tcpSocket;
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket.error());
        return;
    }
}