#include "Server.h"
#include <QtNetwork/qhostaddress.h>
#include <QApplication>

#define PORT 10000

int main(int argc, char *argv[]) {
	//QApplication app(argc, argv);
    Server *server = new Server();
    server->listen(QHostAddress::Any, PORT);
	char c;
	while((c = getchar()) != 'q') ;

	return 0;
}
