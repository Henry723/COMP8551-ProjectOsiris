#include "Models3D.h"

Models3D::Models3D(vector<Model3D> models) {
	this->models = models;
	transformation_matrix = glm::mat4(1.0f);
}

int Models3D::getModelIndex(){
	return currentModelIndex;
}

void Models3D::updateIndex(int frameIndex)
{
	currentModelIndex = frameIndex;
}

glm::mat4 Models3D::getModelMatrix() {
	return transformation_matrix;
}

Model3D* Models3D::getCurrentModel3D() {
	return &models[currentModelIndex];
}

void Models3D::resetModelMatrix() {
	transformation_matrix = glm::mat4(1.0f);
}

void Models3D::translate(glm::vec3 translation) {
	transformation_matrix = glm::translate(transformation_matrix, translation);
}

void Models3D::rotate(glm::vec3 rotationAxis, float degrees) {
	transformation_matrix = glm::rotate(transformation_matrix, degrees, rotationAxis);
}

void Models3D::scale(glm::vec3 scale) {
	transformation_matrix = glm::scale(transformation_matrix, scale);
}

