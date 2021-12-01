#pragma once
#include "../Events/EventSystem.h"
#include "../gamestate.h"
#include <entityx/Event.h>
#include "../components.hpp"
#include <stack>

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

struct PhysicsTest : public System<PhysicsTest>, public Receiver<PhysicsTest>, EntityX {
	void configure(EventManager& em) override; // Subscribes to each input event
	void update(EntityManager& es, EventManager& events, TimeDelta dt) override;
	void receive(const Collision& event); // Gets collisions, toggles canMove bools based on sensors
	void receive(const EndCollision& event); // Gets collisions, toggles canMove bools based on sensors
	void receive(const MoveInput& event); // Toggles movement bools to be picked up by update
	void receive(const AttackInput& event); // Toggles attack bools to be picked up by update
	void receive(const EnemyTurnEnd& event); // Toggles attack bools to be picked up by update
	void receive(const EnemyAttack& event); // Toggles attack bools to be picked up by update

	//Movement input flags
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	//Attack input flags
	bool attackLeft = true;
	bool attackRight = true;
	bool attackUp = true;
	bool attackDown = true;
	//Directional entities relative to body
	Entity* leftEntity = nullptr;
	Entity* rightEntity = nullptr;
	Entity* upEntity = nullptr;
	Entity* downEntity = nullptr;
	bool isMoving = false;
	bool playerTurn = true;

	float timeInterval = 10.2f;
	float timeUntilNextOrder = 10.2f;
	float playerSpeed = 0.5;

	int playerHealth = 3;
};
