#include "Animation.h"

Animation::Animation() {
	//default constructor not meant to do anything except satisfy an error
}
Animation::Animation(string _name, double _length, vector<Keyframe> _keyFrames) : name(_name), length(_length), keyFrames(_keyFrames) {

}

int Animation::getCurrentFrame() {
	return keyFrames[currentFrame].index;	
}

Keyframe Animation::getCurrentKeyframe() {
	return keyFrames[currentFrame];
}

//Gets next key frame
//If its currently on the last keyframe, returns current frame.
Keyframe Animation::getNextKeyframe() {
	if (currentFrame == keyFrames.size() - 1) {
		return keyFrames[currentFrame];
	}
	else {
		return keyFrames[double(currentFrame) + 1];
	}
	
}

void Animation::nextFrame() {
	currentFrame++;
}

double Animation::getLength() {
	return length;
}

void Animation::reset() {
	currentFrame = 0;
}

string Animation::getName() {
	return name;
}

bool Animation::checkLastFrame() {

	return currentFrame == keyFrames.size() - 1;
}