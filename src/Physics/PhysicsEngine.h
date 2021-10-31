#pragma once
#include "../components.hpp"
#include "box2d/box2d.h"
#include <stack>

class PhysicsEngine : public System<PhysicsEngine>
{
public:
	PhysicsEngine();
	~PhysicsEngine();
	void update(EntityManager&, EventManager&, TimeDelta) override;
	b2Body* CreateBody(Entity entity, Rigidbody* body);
	void CleanupBodies();

private:
	b2World* world;
	stack<Rigidbody*> bodiesForDeletion;
};


