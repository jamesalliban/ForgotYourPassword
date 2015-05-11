#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"

using namespace ofxKinectForWindows2::Data;

class KinectManager
{
public:
	void setup();
	void update();
	void draw();
	void drawBodyIndex();
	void drawSkeletons();
	vector<Body> getSkeletonData();
	
	ofxKFW2::Device kinect;
	ICoordinateMapper * coordinateMapper;
	
	bool isDrawBodyIndex;
	bool isDrawSkeletons;
	
	float boundsXMin;
	float boundsXMax;
	float boundsZMin;
	float boundsZMax;
};