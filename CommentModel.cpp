#include "CommentModel.h"

using namespace std;

CommentModel::CommentModel() {
    CommentModel("", -1, -1, 0, "");
}

CommentModel::CommentModel(string content, int userId, int musicId, int thumb, string date) {
    this->content = content;
    this->userId = userId;
    this->musicId = musicId;
    this->thumb = thumb;
    this->date = date;
}

string CommentModel::getContent() {
    return this->content;
}

int CommentModel::getUserId() {
    return this->userId;
}

int CommentModel::getMusicId() {
    return this->musicId;
}

int CommentModel::getThumb() {
    return this->thumb;
}

string CommentModel::getDate() {
    return this->date;
}

Type CommentModel::getModelType() {
    return comment;
}

CommentModel::~CommentModel() {
    
}