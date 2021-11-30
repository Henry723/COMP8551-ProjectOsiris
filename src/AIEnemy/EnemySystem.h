#pragma once
#include "../gamestate.h"
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
	void CheckForPlayer(Rigidbody* rigidbody, CommandFlags* commands);
	vector<CommandFlags::EnemyCommand> AvailableMoves(Rigidbody* rigidbody, CommandFlags* commands);
	void receive(const PlayerTurnEnd& event);

	bool enemyTurn = false;
	bool enemiesFinished = false;
	int currentTurnCounter = 0;
	int movingEnemies = 0;
	float enemySpeed = 0.5;
};