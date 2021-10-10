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
	bool isAttributes = false;
	bool isIndices = false;
	void loadModel(const char* modelSource);
	void processNodeAttributes(aiNode* node, const aiScene* scene);
	vector<float> processMeshAttributes(aiMesh* mesh, const aiScene* scene);
	void processNodeIndices(aiNode* node, const aiScene* scene);
	vector<unsigned int> processMeshIndices(aiMesh* mesh, const aiScene* scene);

public:
	// model data
	vector<vector<float>>modelAttributes;
	vector<vector<unsigned int>>modelIndices;
	// export model attributess
	void getModelAttributes(const char* filePath, float &attributesPointer);
	// export model indices
	void getModelIndices(const char* filePath, unsigned int &indicesPointer);
};