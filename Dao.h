#ifndef _DAO_H_
#define _DAO_H_

#include <iostream>
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
	Model getModel();
	bool addModel(Model&);
	bool modifyModel(Model&);
	bool deleteModel(Model&);
};

#endif
