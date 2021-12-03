#pragma once
#include "Animation.h"
#include <map>
#include <string>
#include "Keyframe.h"

class Animator {

private:
	Animation* currentAnimation;
	double animationTime = 0;
	//map that holds the <string, animation>
	map<string, Animation> animations;
	bool noAnimation = true;

public:
	Animator(vector<Animation> anims);
	void update(double timedelta);
	void increaseAnimationTime(double timedelta);
	void doAnimation(string animName);
	void calculateCurrentAnimationFrame();
	int getCurrentFrameIndex();


};