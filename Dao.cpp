#include "Dao.h"
#include "UserModel.h"
#include "MusicModel.h"
#include "CommentModel.h"
#include "RoleModel.h"
#include "AuthorityModel.h"
#include <QSqlError>

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

Model* Dao::getModelById(Type type, int id) {
	Model *p = NULL;
	QSqlQuery query(db);
	//string sql;
	// char tmp[5];
	// itoa(id, tmp, 10);
	// string tid = tmp;

	switch(type) {
		case user: {
			// prevent sql inject
			query.prepare("SELECT * FROM user WHERE id = :id");
			query.bindValue(":id", id);
			query.exec();
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
			query.prepare("SELECT * FROM music where id = :id");
			query.bindValue(":id", id);
			query.exec();
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
			query.prepare("SELECT * FROM role WHERE id = :id");
			sql += tid;
			query.exec();
			string name;
			if(query.next()) {
				name = query.value(1).toString().toStdString();
				p = new RoleModel(name);
				p->setId(id);
			}
			
			break;
		}

		case authority:	 {
			query.prepare("SELECT * FROM auth WHERE id = :id");
			query.bindValue(":id", id);
			query.exec();
			string name;
			if(query.next()) {
				name = query.value(1).toString().toStdString();
				p = new AuthorityModel(name);
				p->setId(id);
			}

			break;
		}
		case comment: {
			query.prepare("SELECT * FROM comment WHERE id = ");
			query.bindValue("id", id);
			query.exec();
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

Model* Dao::getModelByName(Type type, QString name) {
	Model *p = NULL;
	QSqlQuery query(db);
	//string sql;

	switch(type) {
		case user: {
			query.prepare("SELECT * FROM user WHERE username = :name");
			query.bindValue(":name", name);
			query.exec();
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
			query.prepare("SELECT * FROM music where name = :name");
			query.bindValue(":name", name);
			query.exec();
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
			query.prepare("SELECT * FROM role WHERE rolename = :name");
			query.bindValue(":name", name);
			query.exec();
			int id;
			if(query.next()) {
				id = query.value(0).toInt();
				p = new RoleModel(name);
				p->setId(id);
			}
			break;
		}

		case authority:	{
			query.prepare("SELECT * FROM auth WHERE name = :name");
			query.bindValue(":name", name);
			query.exec();
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

bool Dao::addModel(Model& model) {
	//string sql;
	QSqlQuery query(db);
	switch(model.getModelType()) {
		case user: {	
			UserModel m = dynamic_cast<UserModel&>(model);
			query.prepare("INSERT INTO user (username, password) VALUES (:username, :password)");
			query.bindValue(":username", m.getName().c_str());
			query.bindValue(":password", m.getPass().c_str());
			query.exec();
			break;
		}
			
		case music: {
			MusicModel m = dynamic_cast<MusicModel&>(model);
			query.prepare("INSERT INTO music (name, singer) VALUES (:name, :singer)");
			query.bindValue(":name", m.getName().c_str());
			query.exec();
			break;
		}

		case comment: {
			CommentModel m = dynamic_cast<CommentModel&>(model);
			query.prepare("INSERT INTO comment (content, userId, musicId, thumb, date) VALUES (:content, :userId, :musicId, :thumb, :date)");
			query.bindValue(":content", m.getContent().c_str());
			query.bindValue(":userId", m.getUserId());
			query.bindValue(":musicId", m.getMusicId());
			query.bindValue(":thumb", m.getThumb());
			query.bindValue(":date", m.getDate().c_str());
			query.exec();
			break;
		}
		default: return false;
	}

	return query.lastError().type() == QSqlError::NoError;
}

bool Dao::modifyModel(Model& model) {	
	QSqlQuery query(db);
	switch(model.getModelType()) {
		case user: {
			UserModel m = dynamic_cast<UserModel&>(model);
			// do not support so far
			//query.prepare("UPDATE user SET username = :username");
			break;
		}
			
		case music: {
			MusicModel m = dynamic_cast<MusicModel&>(model);
			//do not support so far
			break;
		}

		case comment: {
			CommentModel m = dynamic_cast<CommentModel&>(model);
			// only support to modify the number of thumb
			query.prepare("UPDATE comment SET thumb = :thumb WHERE id = :id");
			query.bindValue(":thumb", m.getThumb());
			query.bindValue(":id", m.getId());
			break;
		}
		default: return false;
	}
	return query.lastError().type() == QSqlError::NoError;
}

bool Dao::deleteModel(Model& model) {
	QSqlQuery query(db);
	switch(model.getModelType()) {
		case user: {
			UserModel m = dynamic_cast<UserModel&>(model);
			query.prepare("DELETE FROM user WHERE id = :id");
			query.bindValue(":id", m.getId());
			query.exec();
			break;
		}
			
		case music: {
			MusicModel m = dynamic_cast<MusicModel&>(model);
			query.prepare("DELETE FROM music WHERE id = :id");
			query.bindValue(":id", m.getId());
			query.exec();
			break;
		}

		case comment: {
			CommentModel m = dynamic_cast<CommentModel&>(model);
			query.prepare("DELETE FROM comment WHERE id = :id");
			query.bindValue(":id", m.getId());
			query.exec();
			break;
		}
		default: return false;
	}
	return query.lastError().type() == QSqlError::NoError;
}