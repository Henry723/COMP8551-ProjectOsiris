#pragma once

struct Keyframe {
	//timestamp relative to the beginning of the animation
	double timestamp;
	//index corresponding to the pose in the model3D
	int index;
	Keyframe(double _time, int _index) : timestamp(_time), index(_index){}
};
