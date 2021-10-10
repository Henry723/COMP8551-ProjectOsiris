#include "PhysicsEngine.h"
#include <stdio.h>

const float MAX_TIMESTEP = 1.0f / 60.0f; //Timestep for Box2D (60 FPS)
const int NUM_VEL_ITERATIONS = 20; //Velocity iterations
const int NUM_POS_ITERATIONS = 3; //Position iterations

PhysicsEngine::PhysicsEngine()
{
	b2Vec2* gravity = new b2Vec2(0.0f, 0.0f);
	world = new b2World(*gravity);

    test();
}

void PhysicsEngine::update(EntityManager& es, EventManager& ev, TimeDelta dt)
{
    if (world)
    {
        while (dt >= MAX_TIMESTEP)
        {
            world->Step(MAX_TIMESTEP, NUM_VEL_ITERATIONS, NUM_POS_ITERATIONS);
            dt -= MAX_TIMESTEP;
        }
        if (dt > 0.0f)
        {
            world->Step(dt, NUM_VEL_ITERATIONS, NUM_POS_ITERATIONS);
        }
    }
    
    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
    {
        //std::cout << b->GetPosition().x << std::endl;
    }
    
}

void PhysicsEngine::test()
{
	b2BodyDef ballBodyDef;
	b2Body* theBall;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.position.Set(0, 0);
	theBall = world->CreateBody(&ballBodyDef);

    if (theBall)
    {
        //Set shape
        b2CircleShape circle;
        circle.m_p.Set(0, 0);
        circle.m_radius = 5.0f;
        //Set fixture definition
        b2FixtureDef circleFixtureDef;
        circleFixtureDef.shape = &circle;
        circleFixtureDef.density = 1.0f;
        circleFixtureDef.friction = 0.0f;
        circleFixtureDef.restitution = 1.0f;
        theBall->CreateFixture(&circleFixtureDef);
        //Other initial setup
        theBall->SetAwake(true);
        theBall->SetLinearVelocity(b2Vec2(0.005f, 0.0f));
    }

}