#pragma once
#include "../components.hpp"
#include "box2d/box2d.h"

class PhysicsEngine : public System<PhysicsEngine>
{
public:
	PhysicsEngine();
	~ PhysicsEngine();
	void test();
	void update(EntityManager&, EventManager&, TimeDelta) override;
	//PhysicsBody addBody(float size);
	//bool removeBody(PhysicsBody body);
	//void setBodyPosition(PhysicsBody body, Vector Position);
	//void setBodyVelocity(PhysicsBody body, Vector velocity);
	//void setBodyImpulse(PhysicsBody body, Vector force);
	//void setBodyTransform(PhysicsBody body, Vector transform);
	//Vector getBodyPosition(PhysicsBody body);
	//Vector getBodyVelocity(PhysicsBody body);
	//Vector getBodyImpulse(PhysicsBody body);
	//Vector getBodyTransform(PhysicsBody body);
private:
	b2World* world;
};

class Body : public b2Body {
public:
	//b2Body* b2World::CreateBody(const b2BodyDef* def)
	//b2Fixture* b2Body::CreateFixture(const b2FixtureDef* def)
	//void b2World::DestroyBody(b2Body* body)
	//void b2Body::DestroyFixture(b2Fixture* fixture)
private:
	b2World* world;
};


