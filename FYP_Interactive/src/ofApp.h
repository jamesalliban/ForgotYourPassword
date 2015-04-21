#pragma once

#include "ofMain.h"
#include "KinectManager.h"
#include "PoseManager.h"
#include "SceneManager.h"
#include "GUI.h"

			
class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void loadShaders();
	void poseRecognised(Pose & eventPose);
	void keyPressed(int key);
	void audioIn(float * input, int bufferSize, int nChannels);
	void exit();
	
	KinectManager kinectManager;
	PoseManager poseManager;
	SceneManager sceneManager;

	GUI gui;

	bool isPaused;

	ofSoundStream soundStream;
	float smoothAmplitude;
	float frameAtLastSoundEvent;
	float minAmplitudeForEvent;
	bool isSoundEventActive;

	bool isNewPoseHack; // need to use a flag as I can'trecord new pose from audioIn()

	bool isDebugVisible;
};