#ifndef _DAO_H_
#define _DAO_H_

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSql>
#include <QtDebug>
#include "Model.h"
#include <vector>
#include "CommentModel.h"

class Dao {
private:
	QSqlDatabase db;
	//QSqlQuery query;
public:
	void connect();
	void close();
	Model* getModelById(Type type, int id);
	Model* getModelByName(Type type, QString name);
	bool addModel(Model& model);
	bool modifyModel(Model& model);
	bool deleteModel(Model& model);
	std::vector<std::string> getAuthoritiesByUserId(int userId);
	std::vector<int> getMusicsByUserId(int userId);
	std::vector<int> getMusicsByName(QString name);
	std::vector<int> followeesByUserId(int userId);
	std::vector<CommentModel*> getCommentsByMusicId(int musicId);
}; 

#endif
