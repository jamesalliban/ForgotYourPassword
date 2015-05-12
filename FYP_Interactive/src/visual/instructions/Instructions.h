#pragma once

#include "ofMain.h"
#include "Silhouette.h"
#include "ofxSimpleTimer.h"

struct PoseThumbnail
{
	vector<ofPolyline> shapes;
	vector<ofPolyline> holes;
	vector<ofPolyline> shapesOffset;
	vector<ofPolyline> holesOffset;
};

class Instructions
{
public:
	void setup(int _srcW, int _srcH);
	void loadPoses();
	void update();
	void draw();
	void selectNewPoses();
	void startShowTimer(int millis);
	void doIntro(int &args);
	void doOutro(int &args);
	void stopAll();

	int srcW, srcH;
	
	vector<PoseThumbnail> poseThumbs;
	vector<PoseThumbnail*> poseThumbSelection;
	
	Silhouette silhouette;

	ofTrueTypeFont font;
	
	ofxSimpleTimer showInstructionsTimer;
	ofxSimpleTimer hideInstructionsTimer;
	
	bool isActive;
	bool isIntro;
	bool isOutro;
	bool haveTimersBeenSetup;
	float currentAlpha;
	float introSpeed;
	float outroSpeed;
	float displayTime;
	float showLoopDelay;

	float cvThreshold;
	float minContourArea;
	float maxContourArea;
	float contourAmountConsidered;
	bool isFindHoles;
	bool isUseApproximation;
	float resampleAmount;

	float randOffset;
	
	float text1YPos;
	float text2YPos;
	float poseSelectionYPos;
	float poseSelectionSpacing;
	float poseSelectionScale;

	bool isVideoRunning;
	//float 
};