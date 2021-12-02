#include "ScoreTest.h"

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
}

void ScoreTest::configure(EventManager& em) {
	em.subscribe<EntityDestroyedEvent>(*this);
	//em.subscribe<ScoreUpdate>(*this);
}

//When an entity is destroyed, base off of the entity name, it will trigger the flags
void ScoreTest::receive(const EntityDestroyedEvent& events) {
	Entity e = events.entity;
	ComponentHandle<GameObject> object = e.component<GameObject>();
	if (object->name == "treasure") collectedTreasure = true;
	if (object->name == "enemy") killedEnemy = true;
}

//Updating score event
//void ScoreTest::receive(const ScoreUpdate& event) {
//	totalScore += event.score;
//	cout << "Total score: " << totalScore << endl;
//}
//
//int ScoreTest::getScore() {
//	return totalScore;
//}

