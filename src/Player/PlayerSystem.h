#pragma once
#include "../Events/EventSystem.h"
#include "../gamestate.h"
#include <entityx/Event.h>
#include "../components.hpp"
#include <stack>

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

struct PlayerSystem : public System<PlayerSystem>, public Receiver<PlayerSystem>, EntityX {
	void configure(EventManager& em) override; // Subscribes to each input event
	void update(EntityManager& es, EventManager& events, TimeDelta dt) override;
	void AttackChecks(); //Check entity and input flags to perform an attack.
	void ResetFlags(); //Reset movement and attack flags on player turn end.
	void receive(const Collision& event); // Gets collisions, toggles canMove bools based on sensors
	void receive(const EndCollision& event); // Gets collisions, toggles canMove bools based on sensors
	void receive(const MoveInput& event); // Toggles movement bools to be picked up by update
	void receive(const AttackInput& event); // Toggles attack bools to be picked up by update
	void receive(const EnemyTurnEnd& event); // Toggles attack bools to be picked up by update

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
	//Turn based flags
	bool isMoving = false; //Used to check if timer should decrement
	bool playerTurn = true; //Makes sure player cannot act on enemy turn

	float timeInterval = 10.2f; //Timer reset value
	float timeUntilNextOrder = 10.2f; //Initial timer value
	float playerSpeed = 0.5; //Player speed value
};
