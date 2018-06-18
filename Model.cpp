#include "Model.h"

Model::Model() { 
    this->id = -1; 
}

Model::~Model() {

}
	
void Model::setId(int id) {
    this->id = id; 
}

int Model::getId() {
    return this->id; 
}