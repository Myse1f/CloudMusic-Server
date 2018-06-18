#ifndef _COMMENTMODEL_H_
#define _COMMENTMODEL_H_

#include "Model.h"
#include <string>

class CommentModel : public Model {
public:
	CommentModel();
	~CommentModel();
	CommentModel(std::string content, int userId, int musicId, int thumb, std::string date);
	std::string getContent();
	int getUserId();
	int getMusicId();
	int getThumb();
	std::string getDate();
	void like();
	Type getModelType();
private:
	std::string content;
	int userId;
	int musicId;
	int thumb;
	std::string date;
};

#endif
