#include "TurnEvents.h"
#include "../gamestate.h"

void TurnEvents::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	// Only do something if we are running
	if (gameState != GameState::RUNNING) 
		return;

	timeElapsed += dt;
	subtract_time(dt);

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

		/*cout << " Time before next timeout is " << timeUntilNextOrder << endl;*/
	}
}

void TurnEvents::configure(EventManager& em) {
	em.subscribe<PlayerTurnEnd>(*this);
}

void TurnEvents::receive(const PlayerTurnEnd& event) {

	// 2. Reset turn timer.
	//timeUntilNextOrder = 5.0f;
	playerActing = false;
	timeUntilNextOrder = 0;
}

void TurnEvents::action_event(GLFWwindow* window) {
	EventManager* eve = (EventManager*)glfwGetWindowUserPointer(window);

	eve->emit<EnemyDebugInput>(EnemyDebugInput::UP);
}

