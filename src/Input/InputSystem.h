#pragma once
#include "../components.hpp"
#include "../SceneManager/SceneManager.h"
#include "../Events/EventSystem.h"
#include "../gamestate.h"

using namespace std;

class InputSystem : public System<InputSystem>
{
private:
	// Set up the 'groups' of keys using the keycodes
	static vector<int> movementKeys; // W A S D
	static vector<int> attackKeys; // Up Left Down Right arrow keys
	static vector<int> interactKeys; // E button
	static vector<int> controlKeys;
	enum InpDir {
		UP,
		LEFT,
		DOWN,
		RIGHT
	};
	enum IntAct {
		INTERACT
	};
	enum ContInp {
		X,
		Y,
		SPACE,
	};

public:
	void update(EntityManager&, EventManager&, TimeDelta) override;
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
