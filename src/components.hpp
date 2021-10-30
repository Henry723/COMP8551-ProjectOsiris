#pragma once
#include "headers.h"
#include "Renderer3D/Model3D.h"

struct Window
{
  Window(GLFWwindow* _window) : window(_window) {}

  GLFWwindow* window;
};

struct Color
{
  Color(GLfloat _red, GLfloat _green, GLfloat _blue, GLfloat _alpha) : red(_red), green(_green), blue(_blue), alpha(_alpha) {}

  GLfloat red, green, blue, alpha;
};

struct Transform
{
	glm::vec3 position, scale;
	//first 3 values are axis of rotation, last value is rotation degrees
	glm::vec4 rotation;

	Transform(glm::vec3 _position, glm::vec4 _rotation, glm::vec3 _scale) : position(_position), rotation(_rotation), scale(_scale) {}
};