#pragma once

#include "../components.hpp"

class AnimationSystem : public System<AnimationSystem> {

private:

public:

	void update(EntityManager&, TimeDelta);


};