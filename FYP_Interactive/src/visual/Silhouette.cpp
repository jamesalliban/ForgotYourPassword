
#include "Silhouette.h"


void Silhouette::setup(int _srcW, int _srcH)
{
	srcW = _srcW;
	srcH = _srcH;

	isIntro = false;
}


void Silhouette::update(ofPixels & pix)
{

	// morph between user and dancer
	if (isIntro)
	{
		transitionFrameProgress = ofGetFrameNum() - frameAtTransitionStart;
		normTransitionProgress = transitionFrameProgress / tweenFrameTotal;
		if (normTransitionProgress >= 1.0)
		{
			createSilhouette(pix, blobs);
			isIntro = false;
			float ddd;
			ofNotifyEvent(transitionCompleteEvent, ddd);	
		}
		else
		{
			createSilhouette(pix, tweenToPoseblobs);
			calculateMorph();
		}
	}
	else
	{
		createSilhouette(pix, blobs);
	}
}


void Silhouette::createSilhouette(ofPixels & pix, vector<Blob> & _blobs)
{
	cvColourImg.setFromPixels(pix);
	cvGreyImg = cvColourImg;
    cvGreyImg.threshold(cvThreshold); 
	
	contourFinder.findContours(cvGreyImg, minContourArea, maxContourArea, contourAmountConsidered, isFindHoles, isUseApproximation);

	_blobs.clear();

	for (int i = 0; i < contourFinder.blobs.size(); i++)
	{
		Blob blob;
		ofPolyline polyline;
		for (int j = 0; j < contourFinder.blobs[i].pts.size(); j++)
		{
			ofPoint pnt = contourFinder.blobs[i].pts[j];
			pnt.x -= srcW * 0.5;
			pnt.y -= srcH * 0.5;
			polyline.addVertex(pnt);
		}
		polyline = polyline.getResampledByCount(resampleAmount);
		blob.polyline = polyline;
		blob.isHole = contourFinder.blobs[i].hole;
		_blobs.push_back(blob);
	}
}

void Silhouette::calculateMorph()
{
	vector<ofPolyline> tweenFromShapesBySize;
	vector<ofPolyline> tweenToShapesBySize;
	vector<ofPolyline> tweenFromHolesBySize;
	vector<ofPolyline> tweenToHolesBySize;

	// populate vectors with the twettfrom shape and hole blobs
	for (int i = 0; i < tweenFromPoseblobs.size(); i++)
	{
		Blob blob = tweenFromPoseblobs[i];
		if (blob.isHole)
			tweenFromHolesBySize.push_back(blob.polyline);
		else
			tweenFromShapesBySize.push_back(blob.polyline);
	}
	// sort the blobs in order of size
	std::sort(tweenFromHolesBySize.begin(), tweenFromHolesBySize.end(), compareBlobsBySize());
	std::sort(tweenFromShapesBySize.begin(), tweenFromShapesBySize.end(), compareBlobsBySize());
		
	// populate vectors with the twettto shape and hole blobs
	for (int i = 0; i < tweenToPoseblobs.size(); i++)
	{
		Blob blob = tweenToPoseblobs[i];
		if (blob.isHole)
			tweenToHolesBySize.push_back(blob.polyline);
		else
			tweenToShapesBySize.push_back(blob.polyline);
	}
	// sort the blobs in order of size
	std::sort(tweenToHolesBySize.begin(), tweenToHolesBySize.end(), compareBlobsBySize());
	std::sort(tweenToShapesBySize.begin(), tweenToShapesBySize.end(), compareBlobsBySize());
 
	if (tweenFromPoseblobs.size() > 0)
	{
		cout << "1 - " << tweenFromShapesBySize[0].getVertices().size() << " - " << tweenToShapesBySize[0].getVertices().size() << endl;

		// if one polyline has less verties than the other add a vertex to the end of the one with less
		if (tweenFromShapesBySize[0].getVertices().size() != tweenToShapesBySize[0].getVertices().size())
		{
			if (tweenFromShapesBySize[0].getVertices().size() < tweenToShapesBySize[0].getVertices().size())
			{
				int diff = tweenToShapesBySize[0].getVertices().size() - tweenFromShapesBySize[0].getVertices().size();
				for (int i = 0; i < diff; i++)
					tweenFromShapesBySize[0].addVertex(tweenFromShapesBySize[0].getVertices().back());
			}
			
			if (tweenToShapesBySize[0].getVertices().size() < tweenFromShapesBySize[0].getVertices().size())
			{
				int diff = tweenFromShapesBySize[0].getVertices().size() - tweenToShapesBySize[0].getVertices().size();
				for (int i = 0; i < diff; i++)
					tweenToShapesBySize[0].addVertex(tweenToShapesBySize[0].getVertices().back());
			}
		}

		blobs.clear();
		Blob blob;
		blob.isHole = false;
		for (int i = 0; i < tweenFromShapesBySize[0].getVertices().size(); i++)
		{
			ofPoint pntFrom = tweenFromShapesBySize[0].getVertices()[i];
			ofPoint pntTo = tweenToShapesBySize[0].getVertices()[i];
			ofPoint pnt;
			pnt.x = ofLerp(pntFrom.x, pntTo.x, normTransitionProgress);
			pnt.y = ofLerp(pntFrom.y, pntTo.y, normTransitionProgress);
			blob.polyline.addVertex(pnt);
		}
		blobs.push_back(blob);
		//cout << "2 - " << tweenFromShapesBySize[0].getVertices().size() << " - " << tweenToShapesBySize[0].getVertices().size() << endl;
	}
	//float minShapeNum = MIN(tweenFromShapesBySize.size(), tweenToShapesBySize.size());
	//cout << tweenFromShapesBySize[0].getVertices().size() << " - " << tweenToShapesBySize[0].getVertices().size() << endl;
	//cout << tweenFromShapesBySize[0].getVertices().size() << endl;
	//cout << tweenToShapesBySize[0].getVertices().size() << endl;

	cout <<  tweenFromPoseblobs.size() << endl;

}

void Silhouette::draw()
{
	drawSilhouette(blobs);
	if (isIntro)
	{
		//drawSilhouette(tweenFromPoseblobs);
	}
}


void Silhouette::drawSilhouette(vector<Blob> & _blobs)
{
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(position.x * ofGetWidth(), position.y * ofGetHeight());
	ofScale(scale, scale);
	for (int i = 0; i < _blobs.size(); i++)
	{
		vector<ofPoint>& vertices = _blobs[i].polyline.getVertices();
		if (_blobs[i].isHole)
			ofSetColor(*holeColour[0], *holeColour[1], *holeColour[2]);
		else
			ofSetColor(*colour[0], *colour[1], *colour[2]);
		ofBeginShape();
		for(int j = 0; j < vertices.size(); j++) 
			ofVertex(vertices[j]);
		ofEndShape();
	}
	ofPopMatrix();
	ofPopStyle();
}


void Silhouette::startAnimation(ofPixels & pix)
{
	frameAtTransitionStart = ofGetFrameNum();
	isIntro = true;
	createSilhouette(pix, tweenFromPoseblobs);
}

void Silhouette::drawCvGreyImg()
{
	cvGreyImg.draw(0, 0);
}

void Silhouette::drawContour()
{
	contourFinder.draw();
}