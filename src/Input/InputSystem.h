#pragma once
#include "../components.hpp"
#include "../SceneManager/SceneManager.h"
#include "../Events/EventSystem.h"
#include "../gamestate.h"

using namespace std;

class InputSystem : public System<InputSystem>
{
private:
	enum InpDir {
		UP,
		LEFT,
		DOWN,
		RIGHT
	};
	enum IntAct {
		INTERACT
	};

public:
	void update(EntityManager&, EventManager&, TimeDelta) override;
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
