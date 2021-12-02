#pragma once
#include "components.hpp"
#include "Input/InputSystem.h"
#include "Renderer3D/RenderSystem.h"
#include "Physics/PhysicsEngine.h"
#include "UI/UI.h"
#include "Configuration/CfgMgrPhysical.h"
#include "Events/EventSystem.h"
#include "Events/TurnEvents.h"
#include "SceneManager/SceneMgrSys.h"
#include "Player/PlayerSystem.h"
#include "gamestate.h"
#include "Audio/AudioSystem.h"

// TESTING -- Start --
#include "MemoryManager/CMMPoolTester.h"
#include "Input/InputEventTester.h"
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

