#include "PhysicsEngine.h"
#include <stdio.h>

const float MAX_TIMESTEP = 1.0f / 60.0f; //Timestep for Box2D (60 FPS)
const int NUM_VEL_ITERATIONS = 20; //Velocity iterations
const int NUM_POS_ITERATIONS = 3; //Position iterations
stack<Collision> collisions; //Track collisions from the listener

//Struct for b2Body user data
struct CollisionData
{
    Entity e; //Parent entity of the body
};

//Struct for b2Fixture user data
struct FixtureData
{
    string name; //Name given to the collider, used for sensors
};

class CollisionListener : public b2ContactListener
{
public: 
    void BeginContact(b2Contact* contact) 
    {
        //Grab the bodies from the collision
        b2Body* bodyA = contact->GetFixtureA()->GetBody();
        b2Body* bodyB = contact->GetFixtureB()->GetBody();
        //If two bodies exist...
        if (bodyA && bodyB)
        {
            //Get the user data from the bodies
            CollisionData* dataA = (CollisionData*)bodyA->GetUserData().pointer;
            CollisionData* dataB = (CollisionData*)bodyB->GetUserData().pointer;
            //Check if either fixture is a sensor
            if (contact->GetFixtureA()->IsSensor() || contact->GetFixtureB()->IsSensor())
            {
                //If it's a sensor, add sensor names to the collision.
                FixtureData* fA = (FixtureData*)contact->GetFixtureA()->GetUserData().pointer;
                FixtureData* fB = (FixtureData*)contact->GetFixtureB()->GetUserData().pointer;
                collisions.push(Collision(&dataA->e, &dataB->e, fA->name, fB->name));
            }
            else
            {
                //No sensors, regular body collision
                collisions.push(Collision(&dataA->e, &dataB->e));
            }
        }
    }
    void EndContact(b2Contact* contact) {}
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold){}
    void PostSolve(b2Contact* contact, const b2Manifold* oldManifold) {}
};

PhysicsEngine::PhysicsEngine()
{
    //Engine is for top-down games, don't need gravity
	b2Vec2* gravity = new b2Vec2(0.0f, 0.0f);
	world = new b2World(*gravity);
    //Add collision listener
    CollisionListener* collisionListener = new CollisionListener();
    world->SetContactListener(collisionListener);
}

PhysicsEngine::~PhysicsEngine()
{
    if (world) delete world;
}

void PhysicsEngine::update(EntityManager& es, EventManager& ev, TimeDelta dt)
{
    //Grab all entities with rigidbody components
    ComponentHandle<Rigidbody> rigidbody;
    for (Entity entity : es.entities_with_components(rigidbody)) {
        
        Rigidbody* body = entity.component<Rigidbody>().get();
        //If the body has not been created, create it.
        if (!body->isCreated) body->body = CreateBody(entity, body);
        //If the body is marked for deletion, add it to deletion stack.
        else if (body->toDelete)
        {
            bodiesForDeletion.push(body);
        }
        /* This moves the entity Transform based on b2Body->GetPosition.
         * Was having some issues updating based on body user data so this might be needed again.
        else
        {
            b2Vec2 pos = body->body->GetPosition();
            ComponentHandle<Transform> transform = entity.component<Transform>();
            if (transform) transform->position = glm::vec3(pos.x, transform->position.y, pos.y);
        }
        */
        //If the body is being moved by the MoveToPoint function...
        if (body->moveBody)
        {
            //Get distance between points
            glm::vec2 dist = body->dest - body->GetPosition();
            //If distance is close, assume collisions have happened, stop movement
            if (abs(dist.x) < 0.1 && abs(dist.y) < 0.1)
            {
                body->SetVelocity(glm::vec2(0, 0));
                body->SetPosition(body->dest);
                body->moveBody = false;
            }
        }
    }

    //Iterate over the bodies in the simulation
    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
    {
        //Get body position
        b2Vec2 pos = b->GetPosition();
        //Get body data
        CollisionData* bodyData = (CollisionData*)b->GetUserData().pointer;
        //Get entity from body data
        Entity entity = bodyData->e;
        //Get entity's Transform
        if (entity)
        {
            ComponentHandle<Transform> transform = entity.component<Transform>();
            if (transform) transform->position = glm::vec3(pos.x, transform->position.y, pos.y);
        }
        else
        {
            b2BodiesForDeletion.push(b);
        }
    }
    //Clean up any bodies meanth for deletion before world step.
    CleanupBodies();

    //Check if there's collisions
    if (!collisions.empty())
    {
        //Emit the event and remove collision data from stack
        ev.emit(collisions.top());
        collisions.pop();
    }

    //World step
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
}

//Cleanup any bodies for deletion
void PhysicsEngine::CleanupBodies()
{
    //As long as there are bodies for deletion, delete them.
    while (!bodiesForDeletion.empty())
    {
        world->DestroyBody(bodiesForDeletion.top()->body);
        bodiesForDeletion.top()->toDelete = false;
        bodiesForDeletion.pop();
    }
    while (!b2BodiesForDeletion.empty())
    {
        world->DestroyBody(b2BodiesForDeletion.top());
        b2BodiesForDeletion.pop();
    }
}

//Function for creating new bodies and fixtures in the simulation
b2Body* PhysicsEngine::CreateBody(Entity entity, Rigidbody* rb)
{
    b2BodyDef bodyDef;
    b2Body* body;

    //Set body initial position.
    bodyDef.position.Set(rb->position.x, rb->position.y);

    //Set up body data with parent entity
    CollisionData* data = new CollisionData;
    data->e = entity;
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(data);

    //Check rigidbody type and set body type accordingly
    if (rb->type == Rigidbody::ColliderType::PLAYER || rb->type == Rigidbody::ColliderType::ENEMY) bodyDef.type = b2_dynamicBody;
    else if (rb->type == Rigidbody::ColliderType::COLLECTIBLE || rb->type == Rigidbody::ColliderType::WALL) bodyDef.type = b2_staticBody;
    else if (rb->type == Rigidbody::ColliderType::PLAYER_ATTACK || rb->type == Rigidbody::ColliderType::ENEMY_ATTACK) bodyDef.type = b2_kinematicBody;
    else {} //No valid collider type

    //Create body
    body = world->CreateBody(&bodyDef);

    //If body was created properly, add fixtures
    if (body)
    {
        //Iterate over all colliders associated with rigidbody
        for (Collider c : rb->colliders)
        {
            //Create basic fixture data, based on engine use case these shouldn't need to be customizable.
            b2FixtureDef fixture;
            fixture.density = 1.0f;
            fixture.friction = 0.0f;
            fixture.restitution = 0.0f;

            //Set sensor based on collider data.
            fixture.isSensor = c.isSensor;

            //Setup fixture data with name if it was given one
            FixtureData* fData = new FixtureData;
            fData->name = c.type == "body" ? "body" : c.type;
            fixture.userData.pointer = reinterpret_cast<uintptr_t>(fData);

            //Setup box collider
            if (c.shape == Collider::Shape::BOX)
            {
                b2PolygonShape box;
                box.SetAsBox(c.size, c.size);
                fixture.shape = &box;
                body->CreateFixture(&fixture);
            }
            //Setup circle collider
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
    //Set body created to true and return.
    rb->isCreated = true;
    return body;
}