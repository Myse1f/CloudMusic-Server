#include "Server.h"
#include <QtNetwork/qhostaddress.h>

#define PORT 8888

int main() {
    Server *server = new Server();
    server->listen(QHostAddress::Any, PORT);

    return 0;
}