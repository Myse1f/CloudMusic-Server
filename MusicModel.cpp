#include "MusicModel.h"

using namespace std;

MusicModel::MusicModel() {
	MusicModel("", "");
}

MusicModel::MusicModel(string name, string singer) {
	this->name = name;
	this->singer = singer;
}

string MusicModel::getName() {
	return this->name;
}

string MusicModel::getSinger() {
	return this->singer;
}

Type MusicModel::getModelType() {
	return music;
}
