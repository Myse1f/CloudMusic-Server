#include "UserModel.h"

using namespace std;

UserModel::UserModel() {
	UserModel("", "");
}

UserModel::UserModel(string name, string pass) {
	this->username = name;
	this->password = pass;
}

string UserModel::getName() {
	return this->username;
}

string UserModel::getPass() {
	return this->password;
}

Type UserModel::getModelType() {
	return user;
}

UserModel::~UserModel() {
	
}