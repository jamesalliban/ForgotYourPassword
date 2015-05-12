#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"

struct Blob
{
	ofPolyline polyline;
	//ofPolyline polylineWithOffset;
	bool isHole;
};

struct compareBlobsBySize
{
	bool operator() (ofPolyline lhs, ofPolyline rhs) { return lhs.getArea() > rhs.getArea(); }
};


class Silhouette
{
public:
	Silhouette() {};
	~Silhouette() {};

	void setup(int _srcW, int _srcH);
	void update(ofPixels & pix);
	void createSilhouette(ofPixels & pix, vector<Blob> & _blobs);
	void calculateMorph();
	void draw();
	void drawSilhouette(vector<Blob> & _blobs);
	void startAnimation(ofPixels & pix);
	void calculateTotalBoundingBox(ofRectangle & rectangle, vector<Blob> & _blobs);
	void drawCvGreyImg();
	void drawContour();
	void updateVideoProgress(int currentFrame, int totalFrames);
	void startIntroFade();
	void stop();
	
	vector<Blob> blobs;
	vector<Blob> tweenFromPoseblobs;
	vector<Blob> tweenToPoseblobs;
	
	float tweenFrameTotal;
	float frameAtTransitionStart;
	float transitionFrameProgress;
	float normTransitionProgress;
	
	ofRectangle tweenFromBBox;
	ofRectangle tweenToBBox;

	bool hasToBBoxBeenCalculated;
	
	ofEvent<float> transitionCompleteEvent;

	//string type;
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

	bool isIntro;
	bool isIntroFade;
	float alphaFade;
	float outroAnimSpeed;
	float introAnimSpeed;
};