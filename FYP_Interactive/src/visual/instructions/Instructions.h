#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

struct PoseSilhouette
{
	ofPolyline polyline;
};

class Instructions
{
public:
	void setup();
	void update();
	void draw();

	vector<PoseSilhouette> poseImages; 
};