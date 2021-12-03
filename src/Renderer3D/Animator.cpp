#include "Animator.h"

Animator::Animator(vector<Animation> anims) {
	for (Animation a : anims) {
		animations[a.getName()]= a;
	}
	currentAnimation = nullptr;
}

void Animator::doAnimation(string animName) {
	animationTime = 0;
	currentAnimation = &animations[animName];
	animating = true;
}

void Animator::increaseAnimationTime(double timedelta) {
	animationTime += timedelta;

	if (animationTime > currentAnimation->getLength()) {
		//End animation and reset animation
		animationTime = 0;
		animating = false;
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
	if(currentAnimation != nullptr)
		calculateCurrentAnimationFrame();
}

void Animator::calculateCurrentAnimationFrame() {
	//get current frame and next frame
	//Keyframe cur = currentAnimation->getCurrentKeyframe();
	Keyframe next = currentAnimation->getNextKeyframe();
	//compare time with next frame. 
	//If time is after next frame's timestamp, change animations current frame and tell the model3d to index the appropriate model.
	if (!currentAnimation->checkLastFrame() && animationTime > next.timestamp) {
		currentAnimation->nextFrame();
	}
}

int Animator::getCurrentFrameIndex() {
	return currentAnimation->getCurrentFrame();
}
bool Animator::checkAnimating() {
	return animating;
}
