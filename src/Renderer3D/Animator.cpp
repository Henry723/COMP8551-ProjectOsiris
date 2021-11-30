#include "Animator.h"

void Animator::doAnimation(Animation anim) {
	animationTime = 0;
	currentAnimation = &anim;
}

void Animator::increaseAnimationTime(double timedelta) {
	animationTime += timedelta;
}

void Animator::update(double timedelta) {
	if (currentAnimation == nullptr) {
		return;
	}
	increaseAnimationTime(timedelta);
	//calculate current animation pose
	calculateCurrentAnimationFrame();
}

void Animator::calculateCurrentAnimationFrame() {
	//get current frame and next frame

	//compare time with next frame. If time is after next frame's timestamp, change animations current frame and tell the model3d to index the appropriate model.
	

	
}
