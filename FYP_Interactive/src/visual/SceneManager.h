#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxFastFboReader.h"
#include "ScreenRecorder.h"
#include "ofxHapPlayer.h"
#include "ofxCv.h"

using namespace ofxKinectForWindows2::Source;
using namespace ofxCv;

class SceneManager
{
public:
	void setup();
	void update(Depth & depth);
	void draw(Depth & depth);
	void drawDebug(Depth & depth);
	void drawDepth(Depth & depth, int x, int y);
	void drawClippedDepth(int x, int y);

	void loadShaders();
	void toggleRecording();
	void playVideo(int sequenceID);
	void exit();


	ofShader depthShader;
	ofFbo depthFbo;
	ScreenRecorder screenRecorder;
	ofxFastFboReader fastFboReader;
	ofPixels pix;


	
    ofxHapPlayer player;
	ofFbo videoFbo;
	bool isPlayingSequence;
	ContourFinder contourFinder;
	float videoSilhouetteThreshold;

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
	bool isDrawLargeClippedDepth;
	float largeClippingScale;

	bool isRecording;
};