#include "PhysicsEngine.h"
#include <stdio.h>

const float MAX_TIMESTEP = 1.0f / 60.0f; //Timestep for Box2D (60 FPS)
const int NUM_VEL_ITERATIONS = 20; //Velocity iterations
const int NUM_POS_ITERATIONS = 3; //Position iterations

PhysicsEngine::PhysicsEngine()
{
	b2Vec2* gravity = new b2Vec2(0.0f, 0.0f);
	world = new b2World(*gravity);
}

PhysicsEngine::~PhysicsEngine()
{
    if (world) delete world;
}

void PhysicsEngine::update(EntityManager& es, EventManager& ev, TimeDelta dt)
{
    ComponentHandle<Rigidbody> rigidbody;
    for (Entity entity : es.entities_with_components(rigidbody)) {
        Rigidbody* body = entity.component<Rigidbody>().get();
        if (!body->isCreated)
        {
            ComponentHandle<GameObject> comp = entity.component<GameObject>();
            cout << "in update (create): " << comp->name << endl;
            body->body = CreateBody(&entity, body);
        }
        else if (body->toDelete)
        {
            body->toDelete = false;
            bodiesForDeletion.push(body);
        } ///=========================
        else
        {
            b2Vec2 pos = body->body->GetPosition();
            ComponentHandle<Transform> transform = entity.component<Transform>();
            if (transform) transform->position = glm::vec3(pos.x, transform->position.y, pos.y);
        }
        ///=========================
    }

    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
    {
        b2Vec2 pos = b->GetPosition();
        Entity* parent = (Entity*)b->GetUserData().pointer;
        ComponentHandle<Transform> transform = parent->component<Transform>();

        ComponentHandle<GameObject> comp = parent->component<GameObject>();
        //cout << "in update (transform): " << comp->name << endl;

        //cout << "body position: " << pos.x << " " << pos.y << endl;

        //if (transform) transform->position = glm::vec3(pos.x, transform->position.y, pos.y);
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
    ///=========================
    rb->parentEntity = entity;
    ///========================
    ComponentHandle<GameObject> comp = entity->component<GameObject>();
    cout << "in create: " << comp->name << endl;

    if (rb->type == Rigidbody::ColliderType::PLAYER || rb->type == Rigidbody::ColliderType::ENEMY)
        bodyDef.type = b2_dynamicBody;
    else if (rb->type == Rigidbody::ColliderType::COLLECTIBLE || rb->type == Rigidbody::ColliderType::WALL)
        bodyDef.type = b2_staticBody;
    else if (rb->type == Rigidbody::ColliderType::PLAYER_ATTACK || rb->type == Rigidbody::ColliderType::ENEMY_ATTACK)
        bodyDef.type = b2_kinematicBody;
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

    Entity* parent = (Entity*)body->GetUserData().pointer;
    ComponentHandle<Transform> transform = parent->component<Transform>();

    ComponentHandle<GameObject> comp2 = parent->component<GameObject>();
    cout << "in create (cast): " << comp2->name << endl;


    return body;
}