#pragma once

// OpenGL Includes
#include <glad/glad.h> // Always include glad first to get the OpenGL headers required by glfw
#include <GLFW/glfw3.h>

// GL Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Local Library Includes
#include "Shader.h"
#include "ModelImporter.h"
#include "stb_image.h"

// Standard Library Includes
#include <iostream>
#include <vector>

using namespace std;
using std::copy;

//static ModelImporter MODEL_IMP;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 TexCoord;
};

struct Texture {
	unsigned int id;
	string type;
};

struct Model3D {

private:
	unsigned int vbo, ebo;
	//glm::mat4 transformation_matrix;

	vector<float> attributes;
	vector<unsigned int> indices;

	vector<Vertex> vertices;
	vector<Texture> textures;

public:
	unsigned int vao;
	unsigned int texture;
	Shader shader_program;
	unsigned int numIndices;

	// Constructor
	// Model3D(vector<float>& verts, vector<unsigned int>& inds, unsigned int indexCount, const char* vertPath, const char* fragPath, const char* texPath);
	Model3D(const char* modelSource, const char* vertPath, const char* fragPath, const char* texPath, ModelImporter* importer);
	~Model3D();

	// Tranformation Functions (for editing data)
	//void translate(glm::vec3 translation);
	//void rotate(glm::vec3 rotationAxis, float degrees);
	//void scale(glm::vec3 scale);
	//glm::mat4 getModelMatrix();
	//void resetModelMatrix();

	void clear_buffers();

};
