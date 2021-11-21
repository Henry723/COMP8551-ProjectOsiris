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

struct LoadedModelData {
	vector<float> attribs;
	vector<unsigned int> indices;

	LoadedModelData(vector<float>& a, vector<unsigned int>& i) {
		attribs = a;
		indices = i;
	}
};

struct ModelImporter {

private:
	map<const char*, LoadedModelData*> models;
	bool model_loaded(const char* modelSource);

	// Attribute processing
	void processNodeAttributes(aiNode* node, const aiScene* scene, vector<float>& attribArray);
	void processMeshAttributes(aiMesh* mesh, const aiScene* scene, vector<float>& attributes);

	// Index processing
	void processNodeIndices(aiNode* node, const aiScene* scene, vector<unsigned int>& indexArray);
	void processMeshIndices(aiMesh* mesh, const aiScene* scene, vector<unsigned int>& indexArray);

public:
	void loadModel(const char* modelSource, vector<float>& attribArray, vector<unsigned int>& indexArray);
	ModelImporter();
	~ModelImporter();
};