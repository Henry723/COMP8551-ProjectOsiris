#include "ScoreTest.h"
#include "..\SceneManager\SceneManager.h"

void ScoreTest::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	if (collectedTreasure)
	{
		events.emit<ScoreUpdate>(10);
		collectedTreasure = false;
	}
	if (killedEnemy)
	{
		events.emit<ScoreUpdate>(2);
		killedEnemy = false;
	}
	if (capturedKey)
	{
		events.emit<KeyCaptureUpdate>(true);
		isKeyCaptured = true;
		capturedKey = false; // so we do not keep sending the message
	}
}

void ScoreTest::configure(EventManager& em) {
	em.subscribe<EntityDestroyedEvent>(*this);
	em.subscribe<EndCollision>(*this);
}

// When an entity is destroyed, base off of the entity name, 
// it will trigger the flags
void ScoreTest::receive(const EntityDestroyedEvent& events) {
	Entity e = events.entity;
	ComponentHandle<GameObject> object = e.component<GameObject>();
	if (object->name == "treasure") collectedTreasure = true;
	if (object->name == "enemy") killedEnemy = true;
	if (object->name == "key") capturedKey = true;
}

void ScoreTest::receive(const EndCollision& events)
{
	// Lets save some possible time...
	// Currently only need end collision event when isKeyCaptured is true
	if (   isKeyCaptured == true
		// Only game objects should be involved in collisions, 
		// so should be safe to grab these components.
		&& events.a->valid() && events.b->valid()) //Check for valid entities
	{
		ComponentHandle<GameObject> objectA = events.a->component<GameObject>();
		ComponentHandle<GameObject> objectB = events.b->component<GameObject>();
		if (!objectA || !objectB) 
			return; //Invalid collision, gmae objects not found

		// Is player colliding with the door?
		if (   (objectA->name == "player" || objectB->name == "player")
			&& (objectA->name == "door" || objectB->name == "door"))
		{
			// We have a valid collision so next level through the scene manager
			SceneManager& scene = SceneManager::getInstance();
			scene.nextScene();
		}
	}
}