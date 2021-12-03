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
	void CheckForPlayer(Rigidbody* rigidbody, CommandFlags* commands); //Checks for a player/sensor collision
	void AttackPlayer(EventManager& events, Entity* player); //Convenience function for logic to attack player
	vector<CommandFlags::EnemyCommand> AvailableMoves(Rigidbody* rigidbody, CommandFlags* commands); //Gets list of available moves
	void receive(const PlayerTurnEnd& event); //Listens for player turn end.

	bool enemyTurn = false; //Flag for enemy turn update.
	bool enemiesFinished = false; //Check if enemies are finished acting.
	int movingEnemies = 0; //Keep track of moving enemies so as not to end turn early.
	float enemySpeed = 0.5; //Enemy movement speed
};