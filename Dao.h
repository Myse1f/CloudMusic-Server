#ifndef _DAO_H_
#define _DAO_H_

#include <string>
#include <QSqlError>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSql>
#include <QtDebug>
#include "Model.h"

class Dao {
private:
	QDatabase db;
	QSqlQuery query;
public:
	void connect();
	void close();
	Model* getModelById(Type type, int id);
	Model* getModelByName(Type type, std::string name);
	bool addModel(Model& model);
	bool modifyModel(Model& model);
	bool deleteModel(Model& model);
}; 

#endif
