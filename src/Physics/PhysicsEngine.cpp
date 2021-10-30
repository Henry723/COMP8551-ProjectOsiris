#include "PhysicsEngine.h"
#include <stdio.h>

const float MAX_TIMESTEP = 1.0f / 60.0f; //Timestep for Box2D (60 FPS)
const int NUM_VEL_ITERATIONS = 20; //Velocity iterations
const int NUM_POS_ITERATIONS = 3; //Position iterations

class CollisionListener : public b2ContactListener
{
public:
    void BeginContact(b2Contact* contact) {};
    void EndContact(b2Contact* contact) {};
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
    {

    };
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {};
};

PhysicsEngine::PhysicsEngine()
{
	b2Vec2* gravity = new b2Vec2(0.0f, 0.0f);
	world = new b2World(*gravity);
    CollisionListener* collisions = new CollisionListener();
    world->SetContactListener(collisions);
}

PhysicsEngine::~PhysicsEngine()
{
    if (world) delete world;
}

void PhysicsEngine::update(EntityManager& es, EventManager& ev, TimeDelta dt)
{
    ComponentHandle<Rigidbody> pBodies;
    for (Entity entity : es.entities_with_components(pBodies)) {
        Rigidbody* body = entity.component<Rigidbody>().get();
        if (!body->isCreated) body->body = CreateBody(&entity, body);
        if (body->toDelete)
        {
            body->toDelete = false;
            bodiesForDeletion.push(body);
        }
    }

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
        b2Vec2 pos = b->GetPosition();
        Entity* parent = (Entity*)b->GetUserData().pointer;
        ComponentHandle<Transform> transform = parent->component<Transform>();
        if (transform) transform->position = glm::vec3(pos.x, pos.y, transform->position.z);
    }

    for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
    {
        cout << "there's contacts" << endl;
        Entity* a = (Entity*)c->GetFixtureA()->GetUserData().pointer;
        Entity* b = (Entity*)c->GetFixtureA()->GetUserData().pointer;
        ev.emit<Collision>(a, b);
    }

    CleanupBodies();
}

void PhysicsEngine::CleanupBodies()
{
    while (!bodiesForDeletion.empty())
    {
        world->DestroyBody(bodiesForDeletion.top()->body);
        bodiesForDeletion.pop();   
    }
}

b2Body* PhysicsEngine::CreateBody(Entity* entity, Rigidbody* rb)
{   
    b2BodyDef bodyDef;
    b2Body* body;

    bodyDef.position.Set(rb->position.x, rb->position.y);
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(entity);
    if (rb->type == Rigidbody::ColliderType::PLAYER)
    {
        bodyDef.type = b2_dynamicBody;
        //TODO: Set up collision masks
    }
    else if (rb->type == Rigidbody::ColliderType::ENEMY)
    {
        bodyDef.type = b2_dynamicBody;
        //TODO: Set up collision masks
    }
    else if (rb->type == Rigidbody::ColliderType::COLLECTIBLE)
    {
        bodyDef.type = b2_staticBody;
        //TODO: Set up collision masks
    }
    else if (rb->type == Rigidbody::ColliderType::WALL)
    {
        bodyDef.type = b2_staticBody;
        //TODO: Set up collision masks
    }
    else if (rb->type == Rigidbody::ColliderType::PLAYER_ATTACK)
    {
        bodyDef.type = b2_kinematicBody;
        //TODO: Set up collision masks
    }
    else if (rb->type == Rigidbody::ColliderType::ENEMY_ATTACK)
    {
        bodyDef.type = b2_kinematicBody;
        //TODO: Set up collision masks
    }
    else {} //No valid collider type
    
    body = world->CreateBody(&bodyDef);
    if (body)
    {
        b2FixtureDef fixture;
        fixture.density = 1.0f;
        fixture.friction = 0.0f;
        fixture.restitution = 0.0f;
        if (rb->shape == Rigidbody::BodyShape::BOX)
        {
            b2PolygonShape box;
            box.SetAsBox(rb->size, rb->size);
            fixture.shape = &box;
            body->CreateFixture(&fixture);
        }
        else if (rb->shape == Rigidbody::BodyShape::CIRCLE)
        {
            b2CircleShape circle;
            circle.m_p.Set(rb->position.x, rb->position.y);
            circle.m_radius = rb->size;
            fixture.shape = &circle;
            body->CreateFixture(&fixture);
        }
        else {} //No valid body shape 
    }
    
    rb->isCreated = true;
    return body;
}
