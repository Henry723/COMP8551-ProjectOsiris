#pragma once
#include "PhysicsEngine.h"
#include "../Events/EventSystem.h"
#include <entityx/Event.h>
#include "../components.hpp"

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

struct PhysicsTest : public System<PhysicsTest>, public Receiver<PhysicsTest>, EntityX{
	PhysicsTest();
	void configure(EventManager& em) override; // Subscribes to each input event

	void update(EntityManager& es, EventManager& events, TimeDelta dt) override {};

	void receive(const MoveInput& event); // Prints Movement Direction to console on event caught

	Entity* player;
};



