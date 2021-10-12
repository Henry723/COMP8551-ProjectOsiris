#pragma once
#include "../components.hpp"
#include "../SceneManager.hpp"
#include "../Events/EventSystem.h"

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
public:
	void update(EntityManager&, EventManager&, TimeDelta) override;
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
