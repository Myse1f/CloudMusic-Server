#ifndef _ROLEMODEL_H_
#define _ROLEMODEL_H_

#include "Model.h"
#include <string>

class RoleModel : public Model {
public:
    RoleModel();
	~RoleModel();
    RoleModel(std::string name);
    std::string getName();
    Type getModelType();
private:
    std::string name;
};

#endif
