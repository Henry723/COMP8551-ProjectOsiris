#pragma once

#include "../components.hpp"
#include "../Events/EventSystem.h"

class AnimationSystem : public System<AnimationSystem>, public Receiver<AnimationSystem> {

private:
	bool startAttackAnimation = false;
	bool startMoveAnimation = false;
public:

	void update(EntityManager&, EventManager&, TimeDelta) override;
	void configure(EventManager& events) override;
	void receive(const MoveInput& events);
	void receive(const AttackInput& events);

};