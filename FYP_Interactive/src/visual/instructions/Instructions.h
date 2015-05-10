#pragma once

#include "ofMain.h"
#include "Silhouette.h"


struct PoseThumbnail
{
	//Silhouette silhouette;
	//ofPixels pix;
	
	vector<ofPolyline> shapes;
	vector<ofPolyline> holes;
};

class Instructions
{
public:
	void setup(int _srcW, int _srcH);
	void update();
	void draw();
	void selectNewPoses();

	int srcW, srcH;
	
	vector<PoseThumbnail> poseThumbs;
	vector<PoseThumbnail*> poseThumbSelection;
	
	Silhouette silhouette;

	ofTrueTypeFont font;

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
};