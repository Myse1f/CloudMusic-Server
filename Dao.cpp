#include "Dao.h"
#include "UserModel.h"
#include "MusicModel.h"
#include "CommentModel.h"
#include "RoleModel.h"
#include "AuthorityModel.h"

using namespace std;

Dao::Dao() {
	connect();
}

void Dao::connect() {
	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostname("127.0.0.1");
	db.setPort(3306);
	db.setDatabaseName("CloudMusic");
	db.setUserName("root");
	db.setPassword("xxxxxx"); //secret
	if(!db.open()) {
		QDebug() << "Database open failed! Check the password!";
	}
	exit(-1);
}

void Dao::close() {
	db.close();
}

Model* getModelById(Type type, int id) {
	Model *p;
	string sql;
	char tmp[5];
	itoa(id, tmp, 10);
	string tid = tmp;

	switch(type) {
		case user:
			sql = "SELECT * FROM user WHERE id = ";
			sql += tid;
			query.exec(sql.c_str());
			string username, password;
			while(query.next()) {
				username = query.value(1).toString().toStdString();
				password = query.value(2).toString().toStdString();
			}
			p = new UserModel(username, password);
			p->setId(id);
			break;

		case music:
			sql = "SELECT * FROM music where id = ";
			sql += tid;
			query.exec(sql.c_str());
			string name, singer;
			while(query.next()) {
				name = query.value(1).toString().toStdString();
				singer = query.value(2).toString().toStdString();
			}
			p = new MusicModel(name, singer);
			p->setId(id);
			break;

		case role:
			sql = "SELECT * FROM role WHERE id = ";
			break;
		case authority:	
			break;
		case comment:
			break;
		default: break;
	}

	return p;
}