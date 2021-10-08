#pragma once
#include "../components.hpp"
#include "box2d/box2d.h"

class PhysicsEngine : public System<PhysicsEngine>
{
public:
	PhysicsEngine();
	void test();
	void update(EntityManager&, EventManager&, TimeDelta) override;
	b2World* GetWorld() { return world; };
private:
	b2World* world;
};

