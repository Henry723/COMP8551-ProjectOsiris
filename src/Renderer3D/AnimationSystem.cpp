#include "AnimationSystem.h"
#include "Animator.h"

void AnimationSystem::update(EntityManager& es, TimeDelta dt) {

	ComponentHandle<Animator> hAnimators;

	for (Entity entity : es.entities_with_components(hAnimators)) {
		Animator* ator = entity.component<Animator>().get();
		ator->update(dt);
	}
	
}