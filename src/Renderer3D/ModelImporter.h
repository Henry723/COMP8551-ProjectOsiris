#pragma once

// Assimp Includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Stbi includes
#include "stb_image.h"

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
	vector<float>* attribs;
	vector<unsigned int>* indices;

	LoadedModelData(vector<float>& a, vector<unsigned int>& i) {
		attribs = new vector<float>();
		indices = new vector<unsigned int>();

		for (int j = 0; j < a.size(); ++j) {
			attribs->push_back(a[j]);
		}
		/*attribs = a;*/
		for (int j = 0; j < i.size(); ++j) {
			indices->push_back(i[j]);
		}

		/*cout << " In Loaded Model Constructor:	 Attrib size" << attribs->size() << endl;
		cout << "		Indices size" << indices->size() << endl;*/

		// indices = i;
	}

	~LoadedModelData() {
		attribs->clear();
		indices->clear();

		delete attribs;
		delete indices;
	}
};

struct LoadedTextureData {
	unsigned char* textureData;
	int imgWidth, imgHeight, nrChannels;

	LoadedTextureData(const char* texSource) {
		textureData = stbi_load(texSource, &imgWidth, &imgHeight, &nrChannels, 0);
	}

	~LoadedTextureData() {
		//stbi_image_free(textureData);
		delete textureData;
	}
};

struct ModelImporter {

private:
	map<string, LoadedModelData*> models;
	map<string, LoadedTextureData*> textures;

	bool model_loaded(string modelKey);
	bool texture_loaded(string texKey);

	void populate_maps();
	void add_model_to_map(const char* modelSource);
	void add_texture_to_map(const char* texSource);
	

	// Attribute processing
	void processNodeAttributes(aiNode* node, const aiScene* scene, vector<float>& attribArray);
	void processMeshAttributes(aiMesh* mesh, const aiScene* scene, vector<float>& attributes);

	// Index processing
	void processNodeIndices(aiNode* node, const aiScene* scene, vector<unsigned int>& indexArray);
	void processMeshIndices(aiMesh* mesh, const aiScene* scene, vector<unsigned int>& indexArray);

	string key_from_filePath(const char* filePath);

public:
	void get_model(const char* modelSource, vector<float>& attribArray, vector<unsigned int>& indexArray);
	unsigned char* get_texture(const char* texSource, int& width, int& height, int& nrChannels);

	ModelImporter();
	~ModelImporter();
};