#ifndef _DAO_H_
#define _DAO_H_

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
//#include <QtSql/QSql>
#include <QtDebug>
#include "Model.h"
#include <vector>
#include "CommentModel.h"

class Dao {
private:
	QSqlDatabase db;
	//QSqlQuery query;
public:
	Dao();
	void connect();
	void close();
	Model* getModelById(Type type, int id);
	Model* getModelByName(Type type, QString name);
	bool addModel(Model* model);
	bool modifyModel(Model* model);
	bool deleteModel(Model* model);
	void addFollow(int follower, int followee);
	std::vector<int>& getAuthoritiesByUserId(int userId);
	std::vector<int>& getMusicsByUserId(int userId);
	std::vector<int>& getMusicsByName(QString name); // for fuzzy search
	std::vector<int>& getMusicsBySinger(QString singer); //search for singer
	std::vector<int>& getfolloweesByUserId(int userId);
	std::vector<int>& getCommentsByMusicId(int musicId);
}; 

#endif
