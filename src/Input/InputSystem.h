#pragma once
#include "../components.hpp"
#include "../SceneManager.hpp"

class InputSystem : public System<InputSystem>
{
public:
	void update(EntityManager&, EventManager&, TimeDelta) override;
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
