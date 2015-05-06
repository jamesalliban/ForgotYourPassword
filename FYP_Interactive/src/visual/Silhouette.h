#pragma once

#include "ofMain.h"
#include "ofxCv.h"

using namespace ofxCv;

class Silhouette
{
public:
	void setup(int _srcW, int _srcH);
	void update(ofImage & img);
	void draw();
	void drawContour();

	ContourFinder contourFinder;
	float imageContourThreshold;
	float resampleAmount;
	int srcW;
	int srcH;
	ofPoint position;
	float scale;
};