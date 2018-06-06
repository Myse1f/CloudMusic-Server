#ifndef _AUTHORITYMODEL_H_
#define _AUTHORITYMODEL_H_

#include "Model.h"
#include <string>

class AuthorityModel : public Model {
public:
    AuthorityModel();
    AuthorityModel(std::string name);
    std::string getName();
    Type getModelType();
private:
    std::string name;
};

#endif