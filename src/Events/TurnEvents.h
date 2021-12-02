#pragma once
#include "../Events/EventSystem.h"
#include <entityx/Event.h>
#include "../components.hpp"
#include <chrono>

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

struct TurnEvents : public System<TurnEvents>, public Receiver<TurnEvents>, EntityX {

private:
	//ComponentHandle<TurnOrder> turnOrder;
	float timeElapsed = 0;
	float timeInterval = 20.2f;
	float timeUntilNextOrder = 4.2f;

	void subtract_time(TimeDelta dt)
	{
		timeUntilNextOrder -= dt;
	}

	bool time_out()
	{
		return (timeUntilNextOrder <= 0);
	}

	void reset_interval()
	{
		timeUntilNextOrder = timeInterval;
	}

public:
	void configure(EventManager& em) override; // Subscribes to each input event
	void update(EntityManager& es, EventManager& events, TimeDelta dt) override;

	//void receive(const Collision& event); // Gets collisions, toggles canMove bools based on sensors
	void receive(const MoveInput& event); // Toggles movement bools to be picked up by update
	void receive(const AttackInput& event); // Toggles attack bools to be picked up by update

	void receive(const PlayerTurnEnd& event); // Toggles attack bools to be picked up by update
	void action_event(GLFWwindow* window);

	bool playerActing = false;
	bool enemyActing = false;
	

};