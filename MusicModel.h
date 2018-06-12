#ifndef _MUSICMODEL_H_
#define _MUSICMODEL_H_

#include "Model.h"
#include <string>

class MusicModel : public Model {
public:
	MusicModel();
	~MusicModel();
	MusicModel(std::string name, std::string singer);
	std::string getName();
	std::string getSinger();
	Type getModelType();
private:
	std::string name;
	std::string singer;
};

#endif
