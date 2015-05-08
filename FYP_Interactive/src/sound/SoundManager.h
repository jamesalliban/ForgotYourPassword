#pragma once

#include "ofMain.h"


class SoundManager
{
public:
	void setup();
	void update(float currentVideoFrame, float totalVideoFrames);
	void startSound();
	void stopSound();
	void draw();

	ofSoundPlayer soundPlayer;
	bool isPlayingSound;
	float volume;
	float introSpeed;
	float outroSpeed;
};