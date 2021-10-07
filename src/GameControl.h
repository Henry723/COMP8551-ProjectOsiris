#pragma once
#include "components.hpp"
#include "Input/InputSystem.h"
#include "Renderer3D/RenderSystem.h"
#include "Event/EventSystem.h"

// TODO Create Game Control for menus & game
class GameControl : public EntityX
{
public:
  GameControl(GLFWwindow*, string);
  void Update(TimeDelta);
};

