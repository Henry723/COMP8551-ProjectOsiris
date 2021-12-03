#include "ScoreTest.h"
#include "../SceneManager/SceneManager.h"

// Ugly Kludge as the GameObject is always destroying itself and recreating
// on scene change. Consequently all systems are wiped so we don't have data
// that persists unless its global. Will fix in future.
int g_CurScore = 0;
int g_HighScore = 0;
int g_handle = -1;
CCfgMgrApplication g_cfgMgr = CCfgMgrApplication();

void ScoreTest::update(EntityManager& es, EventManager& events, TimeDelta dt)
{
	if (m_CollectedTreasure)
	{
		m_CollectedTreasure = false;
		g_CurScore += 10;
	}
	if (m_KilledEnemy)
	{
		m_KilledEnemy = false;
		g_CurScore += 2;
	}
	if (m_CapturedKey)
	{
		m_CapturedKey = false; // so we do not keep sending the message
		m_IsKeyCaptured = true;
		events.emit<KeyCaptureUpdate>(true);
	}
}

void ScoreTest::configure(EventManager& em) {
	em.subscribe<EntityDestroyedEvent>(*this);
	em.subscribe<Collision>(*this);
}

bool ScoreTest::loadHighScore(void)
{
	bool rslt = false;
	g_HighScore = 0; // Set default for fall back

	// Load the Total Score from config manager
	g_handle = g_cfgMgr.loadCustomData(".\\src\\Stats.txt");
	if (g_handle != -1)
	{
		// Get the value but if it can't load use the default assigned earlier
		if (g_cfgMgr.getDataValue(g_handle, m_Cfg_HSkey, m_Cfg_Val))
		{
			// Have a value so translate it
			g_HighScore = m_Cfg_Val.getValueInt();
			rslt = true;
		}
	}

	return rslt;
}

// Check if the current score beats high score and save to open file handle
bool ScoreTest::evalHighScore(void)
{
	bool rslt = false;

	if (g_HighScore < g_CurScore)
	{
		g_HighScore = g_CurScore;

		// Save its value to our open file. Remember, file not flushed until saved
		// DO NOT FORGET TO CLOSE FILE SO WE CAN REOPEN AFTER SILLY GAMEOBJECT
		// deletes and recreates on scene change
		m_Cfg_Val.setValue(g_HighScore);
		rslt = g_cfgMgr.setDataValue(g_handle, m_Cfg_HSkey, &m_Cfg_Val);
		g_cfgMgr.saveCustomData(g_handle);
	}

	return rslt;
}

// When an entity is destroyed, base off of the entity name, 
// it will trigger the flags
void ScoreTest::receive(const EntityDestroyedEvent& events) {
	Entity e = events.entity;
	ComponentHandle<GameObject> object = e.component<GameObject>();
	if (object->name == "treasure") m_CollectedTreasure = true;
	if (object->name == "enemy") m_KilledEnemy = true;
	if (object->name == "key") m_CapturedKey = true;
}

void ScoreTest::resetScore(void)
{
	g_CurScore = 0;
}

int ScoreTest::getScore(void)
{
	return g_CurScore;
}

int ScoreTest::getHighScore(void)
{
	return g_HighScore;
}

void ScoreTest::receive(const Collision& events)
{
	// Lets save some possible time...
	// Currently only need end collision event when isKeyCaptured is true
	if (   m_IsKeyCaptured == true
		// Only game objects should be involved in collisions, 
		// so should be safe to grab these components.
		&& events.a->valid() && events.b->valid()) //Check for valid entities
	{
		ComponentHandle<GameObject> objectA = events.a->component<GameObject>();
		ComponentHandle<GameObject> objectB = events.b->component<GameObject>();

		if (!objectA || !objectB) 
			return; //Invalid collision, gmae objects not found
		if (objectA->name != "door" && objectB->name != "door") //Make sure we have a door collision
			return;
		if (objectA->name != "player" && objectB->name != "player") //Make sure we have a player collision
			return;

		//Get the fixture name of whichever object isn't the door.
		string OtherFixture = objectA->name == "door" ? events.fB : events.fA;
		
		// Is player colliding with the door?
		if (OtherFixture == "body") //Make sure it's a body fixture
		{
			// We have a valid collision so next level through the scene manager
			SceneManager& scene = SceneManager::getInstance();
			scene.nextScene();
		}
	}
}
