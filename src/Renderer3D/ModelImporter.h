#pragma once

// Assimp Includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// GL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

struct ModelImporter {

private:
	void loadModel(const char* modelSource);
	void processNode(aiNode* node, const aiScene* scene);
	float* processMeshAttributes(aiMesh* mesh, const aiScene* scene);
	unsigned int* processMeshIndices(aiMesh* mesh, const aiScene* scene);

public:
	// model data
	// export model attributess
	float* getModelAttributes(const char* filePath);
	// export model indices
	float* exportModelIndices();
};