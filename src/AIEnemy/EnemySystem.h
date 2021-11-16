#pragma once
#include "../Physics/PhysicsEngine.h"
#include "../Events/EventSystem.h"
#include <entityx/Event.h>
#include "../components.hpp"

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

enum EnemyCommand {
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT
};

struct EnemySystem : public System<EnemySystem>, public Receiver<EnemySystem>, EntityX {
	void configure(EventManager& em) override; // Subscribes to each input event
	void update(EntityManager& es, EventManager& events, TimeDelta dt) override;
	void receive(const Collision& event); // Gets collisions, toggles canMove bools based on sensors
	void receive(const EnemyDebugInput& event);
	void ResetMoveFlags(); //Convenience function for reset collision flags
	void ResetCollisionEntities(); //Convenience function for resetting detected entities

	bool enemyTurn = false;

	// Enemy Command input flags
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	//Attack input flags
	bool attackLeft = true;
	bool attackRight = true;
	bool attackUp = true;
	bool attackDown = true;
	
	//Collision flags for available movement.
	//		These will only be false if walls are detected.
	//		If a player is present, the command can still be performed.
	bool canMoveUp = true;
	bool canMoveDown = true;
	bool canMoveLeft = true;
	bool canMoveRight = true;

	//Directional entities relative to body
	Entity* leftEntity = nullptr;
	Entity* rightEntity = nullptr;
	Entity* upEntity = nullptr;
	Entity* downEntity = nullptr;
};