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

class CollisionListener : public b2ContactListener
{
public: 
    void BeginContact(b2Contact* contact) {};
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
                if (obj) cout << "anem sutf " << obj->name;
                Entity entityb = entityB->e;
                ComponentHandle<GameObject> objb = entityb.component<GameObject>();
                if (objb) cout << "anem bbbb " << objb->name;
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
            b2Vec2 pos = body->body->GetPosition();
            ComponentHandle<Transform> transform = entity.component<Transform>();
            if (transform) transform->position = glm::vec3(pos.x, transform->position.y, pos.y);
        }
        ///=========================
    }

    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
    {
        b2Vec2 pos = b->GetPosition();
        //Entity* parent = (Entity*)b->GetUserData().pointer;
        //ComponentHandle<Transform> transform = parent->component<Transform>();
        //cout << "body at position x=" << pos.x << " y=" << pos.y << endl;
        //if (transform) transform->position = glm::vec3(pos.x, transform->position.y, pos.y);
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

    ///=========================
    //rb->parentEntity = entity;
    ///========================

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
            circle.m_p.Set(0, 0);
            circle.m_radius = rb->size/2;
            fixture.shape = &circle;
            body->CreateFixture(&fixture);
        }
        else {} //No valid body shape 
    }
    rb->isCreated = true;
    return body;
}