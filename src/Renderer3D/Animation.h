#pragma once
#include "Keyframe.h"
#include <vector>
#include <string>

using namespace std;

struct Animation {

private:
	double length;
	//holds keyframes that index the pose
	vector<Keyframe> keyFrames;
	//index of the current Keyframe
	int currentFrame;
	//name of animation
	string name;

public:
	int getCurrentFrame();
	Keyframe getCurrentKeyframe();
	Keyframe getNextKeyframe();
	void nextFrame();
	double getLength();
	void reset();
};