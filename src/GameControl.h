#pragma once
#include "components.hpp"
#include "Input/InputSystem.h"
#include "Renderer3D/RenderSystem.h"
#include "Physics/PhysicsEngine.h"
#include "UI/UI.h"
#include "Configuration/CfgMgrPhysical.h"
#include "Events/SampleEvent.h"

// TESTING -- Start --
#include "MemoryManager/CMMPoolTester.h"
// TESTING -- End --

// TODO Create Game Control for menus & game
class GameControl : public EntityX
{
public:
  GameControl(GLFWwindow*, string);
  void Update(TimeDelta);
};

