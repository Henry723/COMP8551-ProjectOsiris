#include "PhysicsEngine.h"
#include <stdio.h>

const float MAX_TIMESTEP = 1.0f / 60.0f; //Timestep for Box2D (60 FPS)
const int NUM_VEL_ITERATIONS = 20; //Velocity iterations
const int NUM_POS_ITERATIONS = 3; //Position iterations
stack<Collision> collisions;

struct CollisionData
{
    Entity e;
};

struct FixtureData
{
    string name;
};

class CollisionListener : public b2ContactListener
{
public: 
    void BeginContact(b2Contact* contact) 
    {
        //Grab the bodies from the collision
        CollisionData* dataA = (CollisionData*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        CollisionData* dataB = (CollisionData*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
        //If two bodies exist...
        if (dataA && dataB)
        {
            if (contact->GetFixtureA()->IsSensor() || contact->GetFixtureB()->IsSensor())
            {
                FixtureData* fA = (FixtureData*)contact->GetFixtureA()->GetUserData().pointer;
                FixtureData* fB = (FixtureData*)contact->GetFixtureB()->GetUserData().pointer;
                collisions.push(Collision(&dataA->e, &dataB->e, fA->name, fA->name));
            }
            else
            {
                collisions.push(Collision(&dataA->e, &dataB->e));
            }
        }
    };
    void EndContact(b2Contact* contact) {};
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
    {
        b2WorldManifold worldManifold;
        contact->GetWorldManifold(&worldManifold);
        b2PointState state1[2], state2[2];
        b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());
        if (state2[0] == b2_addState)
        {
            //Grab the bodies from the collision
            CollisionData* entityA = (CollisionData*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
            CollisionData* entityB = (CollisionData*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

            //If two bodies exist...
            if (entityA && entityB)
            {
                Entity entity = entityA->e;
                ComponentHandle<GameObject> obj = entity.component<GameObject>();
                Entity entityb = entityB->e;
                ComponentHandle<GameObject> objb = entityb.component<GameObject>();
                collisions.push(Collision(&entityA->e, &entityB->e));
            }
        }
    }
    void PostSolve(b2Contact* contact, const b2Manifold* oldManifold) {};
};

PhysicsEngine::PhysicsEngine()
{
	b2Vec2* gravity = new b2Vec2(0.0f, 0.0f);
	world = new b2World(*gravity);
    CollisionListener* collisionListener = new CollisionListener();
    world->SetContactListener(collisionListener);
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
            body->body = CreateBody(entity, body);

        }
        else if (body->toDelete)
        {
            body->toDelete = false;
            bodiesForDeletion.push(body);
        } ///=========================
        else
        {
            //b2Vec2 pos = body->body->GetPosition();
           //ComponentHandle<Transform> transform = entity.component<Transform>();
            //if (transform) transform->position = glm::vec3(pos.x, transform->position.y, pos.y);
        }
        ///=========================

        if (body->moveBody)
        {
            glm::vec2 dist = body->dest - body->GetPosition();
            if (dist.x < 0.1 && dist.y < 0.1)
            {
                body->SetVelocity(glm::vec2(0, 0));
                body->SetPosition(body->dest);
                body->moveBody = false;
            }
        }
    }

    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
    {
        b2Vec2 pos = b->GetPosition();
        CollisionData* bodyData = (CollisionData*)b->GetUserData().pointer;
        Entity entity = bodyData->e;
        ComponentHandle<Transform> transform = entity.component<Transform>();
        if (transform) transform->position = glm::vec3(pos.x, transform->position.y, pos.y);
    }

    if (!collisions.empty())
    {
        ev.emit(collisions.top());
        collisions.pop();
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

b2Body* PhysicsEngine::CreateBody(Entity entity, Rigidbody* rb)
{
    b2BodyDef bodyDef;
    b2Body* body;
    bodyDef.position.Set(rb->position.x, rb->position.y);

    CollisionData* data = new CollisionData;
    data->e = entity;
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(data);

    if (rb->type == Rigidbody::ColliderType::PLAYER || rb->type == Rigidbody::ColliderType::ENEMY) {
        bodyDef.type = b2_dynamicBody;
    }
    else if (rb->type == Rigidbody::ColliderType::COLLECTIBLE || rb->type == Rigidbody::ColliderType::WALL)
    {
        bodyDef.type = b2_staticBody;
        
    }
    else if (rb->type == Rigidbody::ColliderType::PLAYER_ATTACK || rb->type == Rigidbody::ColliderType::ENEMY_ATTACK)
    {  bodyDef.type = b2_kinematicBody;
}
    else {} //No valid collider type

    body = world->CreateBody(&bodyDef);

    if (body)
    {
        for (Collider c : rb->colliders)
        {
            b2FixtureDef fixture;
            fixture.density = 1.0f;
            fixture.friction = 0.0f;
            fixture.restitution = 0.0f;
            fixture.isSensor = c.isSensor;
            FixtureData* fData = new FixtureData;
            fData->name = c.type != "" ? c.type : "body";
            fixture.userData.pointer = reinterpret_cast<uintptr_t>(fData);
            if (c.shape == Collider::Shape::BOX)
            {
                b2PolygonShape box;
                box.SetAsBox(c.size, c.size);
                fixture.shape = &box;
                body->CreateFixture(&fixture);
            }
            else if (c.shape == Collider::Shape::CIRCLE)
            {
                b2CircleShape circle;
                circle.m_p.Set(c.position.x, c.position.y);
                circle.m_radius = c.size;
                fixture.shape = &circle;
                body->CreateFixture(&fixture);
            }
            else {} //No valid body shape 
        }
    }
    rb->isCreated = true;
    return body;
}