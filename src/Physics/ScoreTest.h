#pragma once
#include "PhysicsTest.h"
#include "PhysicsEngine.h"
#include "../Events/EventSystem.h"
#include <entityx/Event.h>
#include "../components.hpp"

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

struct ScoreUpdate {
	explicit ScoreUpdate(int a) : a(a) {}
	int a;
};

struct ScoreTest : public System<ScoreTest>, public Receiver<ScoreTest>, EntityX {
	void configure(EventManager& em) override; // Subscribes to each input event
	void update(EntityManager& es, EventManager& events, TimeDelta dt) override;
	void receive(const EntityDestroyedEvent& events);
	void receive(const ScoreUpdate& event); 

	int totalScore;

	//Treasure flags
	bool collectedTreasure = false;
};
