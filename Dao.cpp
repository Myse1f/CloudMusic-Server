#include "Dao.h"

Dao::Dao() {
	db = QSqlDatabase::addDatabase("QMYSQL");
}

void Dao::connect() {
	db.setHostname("127.0.0.1");
	db.setPort(3306);
	db.setDatabaseName("CloudMusic");
	db.setUserName("root");
	db.setPassword("210031");
	if(!db.open()) {
		QDebug() << "Database open failed!";
	}
	exit(-1);
}

void Dao::connect() {
	db.close();
}
