#include "TurnEvents.h"

void TurnEvents::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	ComponentHandle<Timer> timer;
	ComponentHandle<TurnOrder> turnOrder;

	// Get the timer entity. 
	for (Entity entity : es.entities_with_components(timer, turnOrder))
	{
		timer = entity.component<Timer>();
		turnOrder = entity.component<TurnOrder>();

		timer->timeElapsed += dt;
		turnOrder->subtract_time(dt);

		if (turnOrder->time_out())
		{
			cout << "== TIME OUT == " << endl;
			cout << "Total time elapsed is " << timer->timeElapsed << endl;
			

			// 1. Send enemy action flag
			

			// 2. Reset turn timer.
			turnOrder->reset_interval();

			cout << " Time before next timeout is " << turnOrder->timeUntilNextOrder << endl;

		}
	}
}

void TurnEvents::configure(EventManager& em) {
	em.subscribe<MoveInput>(*this);
	em.subscribe<AttackInput>(*this);
}

//Receive function just sets boolean flags to be picked up by update loop
void TurnEvents::receive(const MoveInput& event) {
	
	// 2. Reset turn timer.
	//turnOrder->reset_interval();
}

//Receive function just sets boolean flags to be picked up by update loop
void TurnEvents::receive(const AttackInput& event) {
	
	// 2. Reset turn timer.
	//turnOrder->reset_interval();
}

