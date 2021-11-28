#include "TurnEvents.h"

void TurnEvents::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//cout << timeUntilNextOrder << endl;
	if (!playerActing)
	{
		timeElapsed += dt;
		subtract_time(dt);
	}
	

	if (time_out())
	{
		// 1. Send enemy action flag
		events.emit<PlayerTurnEnd>();
		events.emit<EnemyDebugInput>(EnemyDebugInput::UP);
		
		// 2. Reset turn timer.
		reset_interval();
	}
}

void TurnEvents::configure(EventManager& em) {
	em.subscribe<PlayerMoveFinish>(*this);
	em.subscribe<PlayerTurnEnd>(*this);
	em.subscribe<PlayerMoveStart>(*this);
	em.subscribe<EnemyMoveEnd>(*this);
	em.subscribe<EnemyMoveStart>(*this);
}


//Receive function just sets boolean flags to be picked up by update loop
void TurnEvents::receive(const PlayerMoveFinish& event) {

	// 2. Reset turn timer.
	playerActing = false;
}

//Receive function just sets boolean flags to be picked up by update loop
void TurnEvents::receive(const PlayerMoveStart& event) {

	// 2. Reset turn timer.
	playerActing = true;
}

void TurnEvents::receive(const PlayerTurnEnd& event) {

	// 2. Reset turn timer.
	//timeUntilNextOrder = 5.0f;
	playerActing = false;
	timeUntilNextOrder = 0;
}

void TurnEvents::receive(const EnemyMoveStart& event) {

	// 2. Reset turn timer.
	//timeUntilNextOrder = 5.0f;
	enemyActing = true;
}

void TurnEvents::receive(const EnemyMoveEnd& event) {

	// 2. Reset turn timer.
	//timeUntilNextOrder = 5.0f;
	enemyActing = false;
}

void TurnEvents::action_event(GLFWwindow* window) {
	EventManager* eve = (EventManager*)glfwGetWindowUserPointer(window);

	eve->emit<EnemyDebugInput>(EnemyDebugInput::UP);
}

