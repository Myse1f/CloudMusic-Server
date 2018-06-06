#include "AuthorityModel.h"

using namespace std;

AuthorityModel::AuthorityModel() {
    AuthorityModel("");
}

AuthorityModel::AuthorityModel(string name) {
    this->name = name;
}

string AuthorityModel::getName() {
    return this->name;
}

Type AuthorityModel::getModelType() {
    return authority;
}

