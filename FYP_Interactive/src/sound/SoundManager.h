#pragma once

#include "ofMain.h"


class SoundManager
{
public:
	void setup();
	void update();
	void startSound();
	void stopSound();
	void draw();

	ofSoundPlayer soundPlayer;
	bool isPlayingSound;
};