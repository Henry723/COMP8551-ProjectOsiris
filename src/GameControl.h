#pragma once
#include "components.hpp"
#include "Input/Input.h"
#include "Render/Render.h"

// TODO Create Game Control for menus & game
class GameControl : public EntityX
{
public:
  GameControl(GLFWwindow*, string);
  void Update(TimeDelta);
};

