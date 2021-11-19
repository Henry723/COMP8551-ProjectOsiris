#include "TurnEvents.h"

void TurnEvents::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	timeElapsed += dt;
	subtract_time(dt);

	if (time_out())
	{
		cout << "== TIME OUT == " << endl;
		cout << "Total time elapsed is " << timeElapsed << endl;
			

		// 1. Send enemy action flag
		ComponentHandle<Window> hwindow;
		for (Entity entity : es.entities_with_components(hwindow))
		{
			Window* window = entity.component<Window>().get();
			action_event(window->window);
		}

		// 2. Reset turn timer.
		reset_interval();

		cout << " Time before next timeout is " << timeUntilNextOrder << endl;

	}
}

void TurnEvents::configure(EventManager& em) {
	em.subscribe<MoveInput>(*this);
	em.subscribe<AttackInput>(*this);
}

//Receive function just sets boolean flags to be picked up by update loop
void TurnEvents::receive(const MoveInput& event) {
	
	// 2. Reset turn timer.
	timeUntilNextOrder = 0.0f;
}

//Receive function just sets boolean flags to be picked up by update loop
void TurnEvents::receive(const AttackInput& event) {
	
	// 2. Reset turn timer.
	timeUntilNextOrder = 0.0f;
}

void TurnEvents::action_event(GLFWwindow* window) {
	EventManager* eve = (EventManager*)glfwGetWindowUserPointer(window);

	eve->emit<EnemyDebugInput>(EnemyDebugInput::UP);
}

