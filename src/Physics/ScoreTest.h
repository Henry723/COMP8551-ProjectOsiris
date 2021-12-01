#pragma once
#include "PhysicsTest.h"
#include "PhysicsEngine.h"
#include "../Events/EventSystem.h"
#include <entityx/Event.h>
#include "../components.hpp"

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

struct ScoreTest : public System<ScoreTest>, public Receiver<ScoreTest>, EntityX {
	static ScoreTest& instance()
	{
		static ScoreTest instance;
		return instance;
	}
private:
	bool isKeyCaptured = false;
public:
	static ScoreTest& getInstance()
	{
		static ScoreTest instance;
		return instance;
	}
	void configure(EventManager& em) override; // Subscribes to each input event
	void update(EntityManager& es, EventManager& events, TimeDelta dt) override;
	void receive(const EndCollision& events);
	void receive(const EntityDestroyedEvent& events);

	//Flags for score
	bool collectedTreasure = false;
	bool killedEnemy = false;
	bool capturedKey = false;
};
