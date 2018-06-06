#include "RoleModel.h"

using namespace std;

RoleModel::RoleModel() {
    RoleModel("");
}

RoleModel::RoleModel(string name) {
    this->name = name;
}

string RoleModel::getName() {
    return this->name;
}

Type RoleModel::getModelType() {
    return role;
}