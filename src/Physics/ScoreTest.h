#pragma once
#include "PhysicsEngine.h"
#include "../Events/EventSystem.h"
#include <entityx/Event.h>
#include "../components.hpp"
#include "../Configuration/CfgMgrApplication.h"

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

struct ScoreTest : public System<ScoreTest>, public Receiver<ScoreTest>, EntityX {
	static ScoreTest& instance()
	{
		static ScoreTest instance;
		return instance;
	}

private:
	bool m_IsKeyCaptured = false;

	// Config mgr custom data
	CCfgMgrApplication::AKey_t m_Cfg_HSkey = CCfgMgrApplication::AKey_t("High Score", CKVType::Int_t);
	CKeyValue m_Cfg_Val = CKeyValue(m_Cfg_HSkey.second);

	// Flags for score
	bool m_CollectedTreasure = false;
	bool m_KilledEnemy = false;
	bool m_CapturedKey = false;

public:
	static ScoreTest& getInstance()
	{
		static ScoreTest instance;
		return instance;
	}

	void configure(EventManager& em) override; // Subscribes to each input event
	void update(EntityManager& es, EventManager& events, TimeDelta dt) override;
	void receive(const Collision& events);
	void receive(const EntityDestroyedEvent& events);
	void resetScore(void);
	int getScore(void);
	int getHighScore(void);

	// Special that access the outside app file storage
	bool evalHighScore(void);
	bool loadHighScore(void);
};
