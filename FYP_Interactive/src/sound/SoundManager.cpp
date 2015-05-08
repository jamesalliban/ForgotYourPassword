#include "SoundManager.h"


void SoundManager::setup()
{
	soundPlayer.loadSound("audio/new_ambience.aif");
	isPlayingSound = false;
}


void SoundManager::update(float currentVideoFrame, float totalVideoFrames)
{
	if (currentVideoFrame < introSpeed)
	{
		soundPlayer.setVolume(ofMap(currentVideoFrame, 0, introSpeed, 0, 1));
	}
	else if (currentVideoFrame > totalVideoFrames - outroSpeed)
	{
		soundPlayer.setVolume(ofMap(currentVideoFrame, totalVideoFrames - outroSpeed, totalVideoFrames, 1, 0));
	}
	else
	{
		soundPlayer.setVolume(1);
	}
}


void SoundManager::startSound()
{
	cout << "- - - startSound" << endl;
	soundPlayer.setVolume(0);
	soundPlayer.play();
	soundPlayer.setPosition(ofRandom(0.8));
	isPlayingSound = true;
}


void SoundManager::stopSound()
{
	cout << "- - - stopSound" << endl;
	soundPlayer.setVolume(0);
	soundPlayer.stop();
	isPlayingSound = false;
}


void SoundManager::draw()
{
}

