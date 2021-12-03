#include "AnimationSystem.h"
#include "Animator.h"


void AnimationSystem::update(EntityManager& es, EventManager& ev, TimeDelta dt) {

	ComponentHandle<Animator> hAnimators;

	for (Entity entity : es.entities_with_components(hAnimators)) {
		Animator* ator = entity.component<Animator>().get();
		ComponentHandle<GameObject> object = entity.component<GameObject>();
		if (object && object->name == "player") {
			if (startMoveAnimation) {
				ator->doAnimation("move");
				startMoveAnimation = false;
			}
			else if (startAttackAnimation) {
				ator->doAnimation("attack");
				startAttackAnimation = false;
			}
		}
		ator->update(dt);
	}
	
	
}

void AnimationSystem::configure(EventManager& em) {
	em.subscribe<MoveInput>(*this);
	em.subscribe<AttackInput>(*this);
}

void AnimationSystem::receive(const MoveInput& events) {
	if (!startMoveAnimation) {
		startMoveAnimation = true;
	}

}

void AnimationSystem::receive(const AttackInput& events) {
	if (!startAttackAnimation) {
		startAttackAnimation = true;
	}
}
