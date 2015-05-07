#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

struct Blob
{
	ofPolyline polyline;
	bool isHole;
};

class Silhouette
{
public:
	void setup(int _srcW, int _srcH);
	void update(ofPixels & pix);
	void draw();
	void drawCvGreyImg();
	void drawContour();

	string type;
	float * colour[3];
	float * holeColour[3];

	//ContourFinder contourFinder;
	ofxCvContourFinder contourFinder;
	ofxCvColorImage cvColourImg;
	ofxCvGrayscaleImage cvGreyImg;
	float cvThreshold;
	float imageContourThreshold;
	float resampleAmount;
	float minContourArea;
	float maxContourArea;
	float contourAmountConsidered;
	bool isFindHoles;
	bool isUseApproximation;

	int srcW;
	int srcH;
	ofPoint position;
	float scale;
};