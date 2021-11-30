#include "Animation.h"

Keyframe* Animation::getCurrentFrame() {
	//if last frame
	if (currentFrame == keyFrames.size() - 1) {
		return &keyFrames[currentFrame];
	}
	else {
		Keyframe frames[2] = { keyFrames[currentFrame], keyFrames[double(currentFrame)+1] };
		return frames;
	}
	

}