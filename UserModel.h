#ifndef _USERMODEL_H_
#define _USERMODEL_H_

#include "Model.h"
#include <string>

class UserModel : public Model {
public:
	Model();
	Model(std::string name, std::string pass);
	std::string getName();
	std::string getPass();
	Type getModelType();

private:
	std::string username;
	std::string password;
}

#endif
