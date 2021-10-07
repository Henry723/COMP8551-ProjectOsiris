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
	void processMesh(aiMesh* mesh, const aiScene* scene);

public:
	// model data
	// export model attributess
	float* exportModelAttribute();
	// export model indices
	float* exportModelIndices();
};