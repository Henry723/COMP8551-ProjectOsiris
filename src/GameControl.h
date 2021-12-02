#pragma once
#include "components.hpp"
#include "Input/InputSystem.h"
#include "Input/InputEventTester.h"
#include "Renderer3D/RenderSystem.h"
#include "Physics/PhysicsEngine.h"
#include "UI/UI.h"
#include "Configuration/CfgMgrPhysical.h"
#include "Events/EventSystem.h"
#include "Events/TurnEvents.h"
#include "SceneManager/SceneMgrSys.h"

// TESTING -- Start --
#include "MemoryManager/CMMPoolTester.h"
#include "Physics/PhysicsTest.h"
#include "AIEnemy/EnemySystem.h"
#include "Physics/ScoreTest.h"
#include "Configuration/CfgMgrAppTester.h"
// TESTING -- End --

// TODO Create Game Control for menus & game
class GameControl : public EntityX
{
public:
  GameControl(GLFWwindow*, string);
  void Update(TimeDelta);
};

