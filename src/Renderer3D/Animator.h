#pragma once
#include "Animation.h"
#include <map>
#include <string>

class Animator {

private:
	Animation* currentAnimation;
	double animationTime = 0;

	map<string, Animation> animations;

public:
	void update(double timedelta);
	void increaseAnimationTime(double timedelta);
	void doAnimation(Animation animation);
	void calculateCurrentAnimationFrame();

};