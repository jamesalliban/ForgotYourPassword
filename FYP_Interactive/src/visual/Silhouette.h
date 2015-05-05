#pragma once

#include "ofMain.h"
#include "ofxCv.h"

using namespace ofxCv;

class Silhouette
{
public:
	void setup();
	void update(ofImage & img);
	void draw();
	void drawContour();

	ContourFinder contourFinder;
	float imageContourThreshold;

};