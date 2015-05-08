#include "SoundManager.h"


void SoundManager::setup()
{
	soundPlayer.loadSound("audio/new_ambience.aif");
	isPlayingSound = false;
}


void SoundManager::update()
{
	
}


void SoundManager::startSound()
{
	cout << "- - - startSound" << endl;
	soundPlayer.play();
	soundPlayer.setPosition(ofRandom(0.8));
	isPlayingSound = true;
}


void SoundManager::stopSound()
{
	cout << "- - - stopSound" << endl;
	soundPlayer.stop();
	isPlayingSound = false;
}


void SoundManager::draw()
{
}

