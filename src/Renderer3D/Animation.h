#pragma once
#include "Keyframe.h"
#include <vector>
#include <string>

using namespace std;

struct Animation {

private:
	float length;
	vector<Keyframe> keyFrames;
	int currentFrame;
	string name;

public:
	Keyframe* getCurrentFrame();

};