#pragma once
#include "headers.h"

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