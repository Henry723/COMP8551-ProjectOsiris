#include "Animator.h"

void Animator::doAnimation(string animName) {
	animationTime = 0;
	currentAnimation = &animations[animName];
	noAnimation = false;
}

void Animator::increaseAnimationTime(double timedelta) {
	animationTime += timedelta;
	if (animationTime > currentAnimation->getLength()) {
		//End animation and reset animation
		animationTime = 0;
		noAnimation = true;
		currentAnimation->reset();
		currentAnimation = nullptr;
	}
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
	//Keyframe cur = currentAnimation->getCurrentKeyframe();
	Keyframe next = currentAnimation->getNextKeyframe();
	//compare time with next frame. 
	//If time is after next frame's timestamp, change animations current frame and tell the model3d to index the appropriate model.
	if (animationTime > next.timestamp) {
		currentAnimation->nextFrame();
	}
}

int Animator::getCurrentFrameIndex() {
	return currentAnimation->getCurrentFrame();
}
