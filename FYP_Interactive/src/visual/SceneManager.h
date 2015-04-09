#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxFastFboReader.h"
#include "ScreenRecorder.h"
#include "ThreadedImageSequenceLoader.h"

using namespace ofxKinectForWindows2::Source;

class SceneManager
{
public:
	void setup();
	void update(Depth & depth);
	void draw(Depth & depth);
	void drawDebug(Depth & depth);
	void drawDepth(Depth & depth);
	void drawClippedDepth();

	void loadShaders();
	void toggleRecording();
	void playImageSequence(int sequenceID);
	void exit();


	ofShader depthShader;
	ofFbo depthFbo;
	ScreenRecorder screenRecorder;
	ofxFastFboReader fastFboReader;
	ofPixels pix;
	ofImage currentLoadedImg;
	
	ThreadedImageSequenceLoader imgSequenceLoader;
	bool isPlayingSequence;

	bool isClipping;

	float liveNearClip;
	float liveFarClip;
	float liveNearFloorClip;
	float liveFarFloorClip;
	float liveNearCeilingClip;
	float liveFarCeilingClip;
	float liveNearLeftClip;
	float liveFarLeftClip;
	float liveNearRightClip;
	float liveFarRightClip;
	
	float recNearClip;
	float recFarClip;
	float recNearFloorClip;
	float recFarFloorClip;
	float recNearCeilingClip;
	float recFarCeilingClip;
	float recNearLeftClip;
	float recFarLeftClip;
	float recNearRightClip;
	float recFarRightClip;
	
	bool isDrawDepth;
	bool isDrawClippedDepth;
	bool isLiveClipping;

	bool isRecording;
};