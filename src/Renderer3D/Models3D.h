#pragma once
#include "Model3D.h"
#include <vector>
#include <glm/glm.hpp>

struct Models3D {

private: 
	vector<Model3D> models;
	int currentModelIndex = 0;
	glm::mat4 transformation_matrix;


public:
	Models3D(vector<Model3D> models);
	int getModelIndex();
	glm::mat4 getModelMatrix();
	void resetModelMatrix();
	Model3D* getCurrentModel3D();
	void translate(glm::vec3 translation);
	void rotate(glm::vec3 rotationAxis, float degrees);
	void scale(glm::vec3 scale);

};
