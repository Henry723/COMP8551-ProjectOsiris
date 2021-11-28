#pragma once
#include "../Physics/PhysicsEngine.h"
#include "../Events/EventSystem.h"
#include <entityx/Event.h>
#include "../components.hpp"
#include <thread>
using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;



struct EnemySystem : public System<EnemySystem>, public Receiver<EnemySystem>, EntityX {
	void configure(EventManager& em) override; // Subscribes to each input event
	void update(EntityManager& es, EventManager& events, TimeDelta dt) override;
	void receive(const Collision& event); // Gets collisions, toggles canMove bools based on sensors
	void receive(const EndCollision& event);
	void receive(const PlayerTurnEnd& event);

	bool enemyTurn = false;
	int currentTurnCounter = 0;
	int movingEnemies = 0;
	float enemySpeed = 0.5;

};