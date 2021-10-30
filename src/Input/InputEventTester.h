#pragma once
#include "../components.hpp"
#include "../Events/EventSystem.h"
#include <entityx/Event.h>
#include "../UI/UI.h"

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

// Acting as tester class to Input and Event systems.
// Listening for input events and printing results to console.
struct InputEventTester : public System<InputEventTester>, public Receiver<InputEventTester> {
	void configure(EventManager& em) override; // Subscribes to each input event

	void update(EntityManager& es, EventManager& events, TimeDelta dt) override {};

	void receive(const MoveInput& event); // Prints Movement Direction to console on event caught

	void receive(const AttackInput& event);// Prints Attack Direction to console on event caught
};