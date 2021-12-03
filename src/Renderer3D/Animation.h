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
	int currentFrame = 0;
	//name of animation
	string name;

public:
	Animation(string _name, double _length, vector<Keyframe> _keyFrames) : name(_name), length(_length), keyFrames(_keyFrames){}
	int getCurrentFrame();
	Keyframe getCurrentKeyframe();
	Keyframe getNextKeyframe();
	void nextFrame();
	double getLength();
	void reset();
	string getName();
};