#include "Server.h"
#include <QtNetwork/qhostaddress.h>
#include <QCoreApplication>

#define PORT 10000

int main(int argc, char *argv[]) {
	QCoreApplication app(argc, argv);
    Server *server = new Server();
    server->run();
	return app.exec();
}
