#ifndef _MODEL_H_
#define _MODEL_H_

typedef enum Type_ {
	user,
	role,
	authority,
	music,
	comment
} Type;

class Model {
public:
	Model() { this->id = -1; }
	virtual ~Model() { };
	virtual Type getModelType() = 0;
	void setId(int id) { this->id = id; }
	int getId() { return this->id; }
protected:
	int id;
};

#endif
