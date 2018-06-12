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
	Model *p = NULL;
	string sql;
	char tmp[5];
	itoa(id, tmp, 10);
	string tid = tmp;

	switch(type) {
		case user: {
			sql = "SELECT * FROM user WHERE id = ";
			sql += tid;
			query.exec(sql.c_str());
			string username, password;
			if(query.next()) {
				username = query.value(1).toString().toStdString();
				password = query.value(2).toString().toStdString();
				p = new UserModel(username, password);
				p->setId(id);
			}
		}
			
			break;

		case music: {
			sql = "SELECT * FROM music where id = ";
			sql += tid;
			query.exec(sql.c_str());
			string name, singer;
			if(query.next()) {
				name = query.value(1).toString().toStdString();
				singer = query.value(2).toString().toStdString();
				p = new MusicModel(name, singer);
				p->setId(id);
			}
			
			break;
		}

		case role: {
			sql = "SELECT * FROM role WHERE id = ";
			sql += tid;
			query.exec(sql.c_str());
			string name;
			if(query.next()) {
				name = query.value(1).toString().toStdString();
				p = new RoleModel(name);
				p->setId(id);
			}
			
			break;
		}

		case authority:	 {
			sql = "SELECT * FROM auth WHERE id = ";
			sql += tid;
			query.exec(sql.c_str());
			string name;
			if(query.next()) {
				name = query.value(1).toString().toStdString();
				p = new AuthorityModel(name);
				p->setId(id);
			}

			break;
		}
		case comment: {
			sql = "SELECT * FROM comment WHERE id = ";
			sql += tid;
			query.exec(sql.c_str());
			string content, date;
			int musicId, userId, thumb;
			if(query.next()) {
				content = query.value(1).toString().toStdString();
				musicId = query.value(2).toInt();
				userId = query.value(3).toInt();
				thumb = query.value(4).toInt();
				date = query.value(5).toString().toStdString();
				p = new CommentModel(content, userId, musicId, thumb, date);
				p->setId(id);
			}

			break;
		}

		default: break;
	}

	return p;
}

Model* getModelByName(Type type, string name) {
	Model *p = NULL;
	string sql;

	switch(type) {
		case user: {
			sql = "SELECT * FROM user WHERE username = '";
			sql += name + "'";
			string password;
			int id;
			if(query.next()) {
				id = query.value(0).toInt();
				password = query.value(2).toString().toStdString();
				p = new UserModel(name, password);
				p->setId(id);
			}
	
			break;
		}

		case music: {
			sql = "SELECT * FROM music where name = '";
			sql += name + "'";
			query.exec(sql.c_str());
			string singer;
			int id;
			if(query.next()) {
				id = query.value(0).toInt();
				singer = query.value(2).toString().toStdString();
				p = new MusicModel(name, singer);
				p->setId(id);
			}
			break;
		}

		case role: {
			sql = "SELECT * FROM role WHERE rolename = '";
			sql += name + "'";
			query.exec(sql.c_str());
			int id;
			if(query.next()) {
				id = query.value(0).toInt();
				p = new RoleModel(name);
				p->setId(id);
			}
			break;
		}

		case authority:	{
			sql = "SELECT * FROM auth WHERE name = '";
			sql += name + "'";
			query.exec(sql.c_str());
			int id;
			while(query.next()) {
				id = query.value(0).toInt();
				p = new AuthorityModel(name);
				p->setId(id);
			}
			break;
		}
		default: break;
	}

	return p;
}