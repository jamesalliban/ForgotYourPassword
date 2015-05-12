
#include "Silhouette.h"


void Silhouette::setup(int _srcW, int _srcH)
{
	srcW = _srcW;
	srcH = _srcH;

	isIntro = false;
	hasToBBoxBeenCalculated = true;
	isIntroFade = false;
	alphaFade = 1.0;
}


void Silhouette::update(ofPixels & pix)
{
	if (isIntroFade)
	{
		alphaFade += introAnimSpeed;
		if (alphaFade >= 1.0)
		{
			alphaFade = 1.0;
			isIntroFade = false;
		}
	}

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
			if (!hasToBBoxBeenCalculated)
			{
				hasToBBoxBeenCalculated = true;
				calculateTotalBoundingBox(tweenToBBox, tweenToPoseblobs);
			}
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
			pnt *= scale;
			pnt.x += position.x * ofGetWidth();
			pnt.y += position.y * ofGetHeight();
			
			//float bboxXOffset = (tweenToBBox.x + tweenToBBox.width * 0.5) - (tweenFromBBox.x + tweenFromBBox.width * 0.5);
			////float bboxXOffset = (tweenFromBBox.x + tweenFromBBox.width * 0.5) - (tweenToBBox.x + tweenToBBox.width * 0.5);
			//if (j == 1)
			//{
			//	cout << "to   x:" << tweenToBBox.x << ", w:" << tweenToBBox.width << endl;
			//	cout << "from x:" << tweenFromBBox.x << ", w:" << tweenFromBBox.width << endl;
			//	//cout << bboxXOffset << endl;
			//}
			//pnt.x += bboxXOffset;
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

	// populate vectors with the tweenfrom shape and hole blobs
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
		
	// populate vectors with the tweento shape and hole blobs
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
		
		//for (int i = 0; i < tweenFromHolesBySize.size(); i++)
		//{

		//	for (int j = 0; j < tweenFromHolesBySize[i].getVertices().size(); j++)
		//	{
		//		ofPoint pntFrom = tweenFromHolesBySize[i].getVertices()[j];
		//		ofPoint pntTo = tweenFromHolesBySize[i].getBoundingBox().getCenter();
		//		ofPoint pnt;
		//		pnt.x = ofLerp(pntFrom.x, pntTo.x, normTransitionProgress);
		//		pnt.y = ofLerp(pntFrom.y, pntTo.y, normTransitionProgress);
		//		blob.polyline.addVertex(pnt);
		//	}
		//}

		
		
		//for (int i = 0; i < tweenToHolesBySize.size(); i++)
		//{

		//	for (int j = 0; j < tweenToHolesBySize[i].getVertices().size(); j++)
		//	{
		//		ofPoint pntFrom = tweenToHolesBySize[i].getBoundingBox().getCenter();
		//		ofPoint pntTo = tweenToHolesBySize[i].getVertices()[j];
		//		ofPoint pnt;
		//		pnt.x = ofLerp(pntFrom.x, pntTo.x, normTransitionProgress);
		//		pnt.y = ofLerp(pntFrom.y, pntTo.y, normTransitionProgress);
		//		blob.polyline.addVertex(pnt);
		//	}
		//}


		blobs.push_back(blob);
	}
}

void Silhouette::draw()
{
	drawSilhouette(blobs);
}


void Silhouette::drawSilhouette(vector<Blob> & _blobs)
{
	ofPushStyle();
	ofPushMatrix();
	for (int i = 0; i < _blobs.size(); i++)
	{
		vector<ofPoint>& vertices = _blobs[i].polyline.getVertices();
		
		if (_blobs[i].isHole)
			ofSetColor(*holeColour[0], *holeColour[1], *holeColour[2], 255);
		else
			ofSetColor(*colour[0], *colour[1], *colour[2], 255 * alphaFade);
		ofBeginShape();
		for(int j = 0; j < vertices.size(); j++) 
		{	
			ofVertex(vertices[j]);
		}
		ofEndShape();
	}
	ofPopMatrix();
	ofPopStyle();
}


void Silhouette::startAnimation(ofPixels & pix)
{
	frameAtTransitionStart = ofGetFrameNum();
	isIntro = true;
	hasToBBoxBeenCalculated = false;
	createSilhouette(pix, tweenFromPoseblobs);

	calculateTotalBoundingBox(tweenFromBBox, tweenFromPoseblobs);
}


void Silhouette::calculateTotalBoundingBox(ofRectangle & rectangle, vector<Blob> & _blobs)
{
	// calculate total bounding box
	ofRectangle bboxTotal;
	for (int i = 0; i < _blobs.size(); i++)
	{
		ofRectangle bbox = _blobs[i].polyline.getBoundingBox();
		if (i == 0)
		{
			rectangle = bbox;
		}
		else
		{
			if (bbox.x < rectangle.x) rectangle.x = bbox.x;
			if (bbox.y < rectangle.y) rectangle.y = bbox.y;
			if (bbox.width < rectangle.width) rectangle.width = bbox.width;
			if (bbox.height < rectangle.height) rectangle.height = bbox.height;
		}
	}
}


void Silhouette::drawCvGreyImg()
{
	cvGreyImg.draw(0, 0);
}

void Silhouette::drawContour()
{
	contourFinder.draw();
}

void Silhouette::updateVideoProgress(int currentFrame, int totalFrames)
{
	alphaFade = ofMap(currentFrame, totalFrames - outroAnimSpeed, totalFrames, 1.0, 0.0, true);
}

void Silhouette::startIntroFade()
{
	isIntroFade = true;
	alphaFade = 0;
}

void Silhouette::stop()
{
	isIntro = false;
	blobs.clear();
	tweenFromPoseblobs.clear();
	tweenToPoseblobs.clear();
}