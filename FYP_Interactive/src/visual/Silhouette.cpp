
#include "Silhouette.h"


void Silhouette::setup(int _srcW, int _srcH)
{
	srcW = _srcW;
	srcH = _srcH;
}


void Silhouette::update(ofPixels & pix)
{
	cvColourImg.setFromPixels(pix);
	cvGreyImg = cvColourImg;
    cvGreyImg.threshold(cvThreshold); 
	
	contourFinder.findContours(cvGreyImg, minContourArea, maxContourArea, contourAmountConsidered, isFindHoles, isUseApproximation);
}


void Silhouette::draw()
{
	vector<Blob> blobs;

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
		blobs.push_back(blob);
	}

	ofPushStyle();
	ofPushMatrix();
	ofTranslate(position.x * ofGetWidth(), position.y * ofGetHeight());
	ofScale(scale, scale);
	for (int i = 0; i < blobs.size(); i++)
	{
		vector<ofPoint>& vertices = blobs[i].polyline.getVertices();
		if (blobs[i].isHole)
			ofSetColor(*holeColour[0], *holeColour[1], *holeColour[2]);
		else
			ofSetColor(*colour[0], *colour[1], *colour[2]);
		ofBeginShape();
		for(int j = 0; j < vertices.size(); j++) 
			ofVertex(vertices[j]);
		ofEndShape();
		//polylines[i].draw();
	}
	ofPopMatrix();
	ofPopStyle();
}


void Silhouette::drawCvGreyImg()
{
	cvGreyImg.draw(0, 0);
}

void Silhouette::drawContour()
{
	contourFinder.draw();
}