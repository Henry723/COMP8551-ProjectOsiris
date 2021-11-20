#pragma once
#include "PhysicsEngine.h"
#include "../Events/EventSystem.h"
#include <entityx/Event.h>
#include "../components.hpp"

#include "../gamestate.h"

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

struct PhysicsTest : public System<PhysicsTest>, public Receiver<PhysicsTest>, EntityX {
	void configure(EventManager& em) override; // Subscribes to each input event
	void update(EntityManager& es, EventManager& events, TimeDelta dt) override;
	void receive(const Collision& event); // Gets collisions, toggles canMove bools based on sensors
	void receive(const MoveInput& event); // Toggles movement bools to be picked up by update
	void ResetMoveFlags(); //Convenience function for reset collision flags

	//Some bools to track input status and collision based movement
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;

	bool canMoveLeft = true;
	bool canMoveRight = true;
	bool canMoveUp = true;
	bool canMoveDown = true;
};
