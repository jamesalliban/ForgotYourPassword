#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxFastFboReader.h"
#include "ScreenRecorder.h"
#include "ofxHapPlayer.h"
#include "Silhouette.h"
#include "Instructions.h"

using namespace ofxKinectForWindows2::Source;

class SceneManager
{
public:
	void setup();
	void setupInstructions();
	void update(Depth & depth, bool isPaused);
	void drawBackground();
	void draw(Depth & depth);
	void drawDebug(Depth & depth);
	void drawDepth(Depth & depth, int x, int y);
	void drawClippedDepth(int x, int y);

	void loadShaders();
	void toggleRecording();
	void playVideo(int sequenceID);
	void stopVideo();
	void pauseVideo();
	void resumeVideo();
	void scrubVideo(int offset);
	int getSequenceSize();
	void exit();


	ofShader depthShader;

	Silhouette userSilhouette;
	Silhouette dancerSilhouette;

	ofFbo depthFbo;
	ScreenRecorder screenRecorder;
	ofxFastFboReader fastFboReader;
	ofPixels pix;

    ofxHapPlayer player;
	
	Instructions instructions;

	ofEvent<float> videoCompleteEvent;

	ofFbo dancerFbo[2];
	ofFbo userFbo[2];
	int currentVidFbo;
	int currentUserFbo;
	//ofImage dancerImg;
	//ofImage userImg;
	ofPixels dancerPix;
	ofPixels userPix;

	bool isPlayingSequence;
	int srcW;
	int srcH;

	
	ofShader blurShaderH;
	ofShader blurShaderV;

	float dancerBlurAmount;
	float dancerBlurSamples;
	float userBlurAmount;
	float userBlurSamples;
	
	float vidScale;
	bool isDancerSilhouetteDebugVisible;
	bool isUserSilhouetteDebugVisible;
	bool isDancerVisible;
	bool isUserVisible;
	
	float bgCol[3];
	float silhouetteCol[3];


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