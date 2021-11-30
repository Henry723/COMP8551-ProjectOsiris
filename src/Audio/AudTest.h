#pragma once
#include "AudioSystem.h"
#include "../Events/EventSystem.h"

using namespace std;

class AudTest : public System<AudTest> , public Receiver<AudTest>{
	
	AudioSystem* aud;

	AudTest();
	~AudTest();
	void configure(EventManager& em) override;
	void update(EntityManager& es, EventManager& events, TimeDelta dt) override {};
	void receive(const AttackInput& event);
};