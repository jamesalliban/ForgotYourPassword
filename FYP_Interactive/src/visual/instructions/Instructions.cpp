#include "Instructions.h"

void Instructions::setup(int _srcW, int _srcH)
{
	srcW = _srcW;
	srcH = _srcH;
	ofDirectory dir;
	dir.listDir("images");

	font.loadFont("fonts/HelveticaNeue-Light.ttf", 32, true, true);

	silhouette.cvThreshold = cvThreshold;
	silhouette.resampleAmount = resampleAmount;
	silhouette.minContourArea = minContourArea;
	silhouette.maxContourArea = maxContourArea;
	silhouette.contourAmountConsidered = contourAmountConsidered;
	silhouette.isFindHoles = isFindHoles;
	silhouette.isUseApproximation = isUseApproximation;


	poseThumbs.clear();
	for (int i = 0; i < dir.getFiles().size(); i++)
	{
		PoseThumbnail poseThumb;
		ofImage img;
		img.loadImage(dir.getFile(i));
		ofPixels pix;
		img.getTextureReference().readToPixels(pix);
		silhouette.update(pix);

		for (int j = 0; j < silhouette.contourFinder.blobs.size(); j++)
		{
			ofPolyline polyline;

			for (int k = 0; k < silhouette.contourFinder.blobs[j].pts.size(); k++)
			{
				ofPoint pnt = silhouette.contourFinder.blobs[j].pts[k];
				pnt.x -= srcW * 0.5;
				pnt.y -= srcH * 0.5;
				polyline.addVertex(pnt);
			} 

			polyline = polyline.getResampledByCount(silhouette.resampleAmount);

			if (silhouette.contourFinder.blobs[j].hole)
				poseThumb.holes.push_back(polyline);
			else
				poseThumb.shapes.push_back(polyline);
		}

		poseThumbs.push_back(poseThumb);
	}
}


void Instructions::update()
{
	if (ofGetFrameNum() % 30 == 0)
		selectNewPoses();
}


void Instructions::draw()
{
	ofPushStyle();
	for (int i = 0; i < poseThumbSelection.size(); i++)
	{
		ofPushMatrix();
		int x = ofMap(i, 0, 2, (ofGetWidth() * 0.5) - poseSelectionSpacing, (ofGetWidth() * 0.5) + poseSelectionSpacing);
		ofTranslate(x, poseSelectionYPos);
		ofScale(poseSelectionScale, poseSelectionScale);

		ofSetColor(255);
		for (int j = 0; j < poseThumbSelection[i]->shapes.size(); j++) 
		{
			vector<ofPoint>& vertices = poseThumbSelection[i]->shapes[j].getVertices();
			ofBeginShape();
			for (int k = 0; k < vertices.size(); k++) 
			{
				ofPoint pnt = vertices[k];
				pnt.x += ofRandomf() * randOffset;
				pnt.y += ofRandomf() * randOffset;
				ofVertex(pnt);
			}
			ofEndShape();
		}
		ofSetColor(0);
		for (int j = 0; j < poseThumbSelection[i]->holes.size(); j++) 
		{
			vector<ofPoint>& vertices = poseThumbSelection[i]->holes[j].getVertices();
			ofBeginShape();
			for (int k = 0; k < vertices.size(); k++) 
			{
				ofPoint pnt = vertices[k];
				pnt.x += ofRandomf() * randOffset;
				pnt.y += ofRandomf() * randOffset;
				ofVertex(pnt);
			}
			ofEndShape();
		}

		ofSetColor(0);
		for (int j = 0; j < poseThumbSelection[i]->shapes.size(); j++) 
			poseThumbSelection[i]->shapes[j].draw();

		ofPopMatrix();
	}

	ofSetColor(255);
	
	string text1 = "unlock your avatar";
	ofRectangle textRect1 = font.getStringBoundingBox(text1, 0, 0);
	ofPushMatrix();
	ofTranslate(ofGetWidth() * 0.5 - textRect1.getWidth() * 0.5, text1YPos);
	font.drawString(text1, 0, 0);
	ofPopMatrix();
	
	
	string text2 = "adopt a pose";
	ofRectangle textRect2 = font.getStringBoundingBox(text2, 0, 0);
	ofPushMatrix();
	ofTranslate(ofGetWidth() * 0.5 - textRect2.getWidth() * 0.5, text2YPos);
	font.drawString(text2, 0, 0);
	ofPopMatrix();

	ofPopStyle();
}


void Instructions::selectNewPoses()
{
	poseThumbSelection.clear();
	for (int i = 0; i < poseThumbs.size(); i++)
		poseThumbSelection.push_back(&poseThumbs[i]);

	random_shuffle(poseThumbSelection.begin(), poseThumbSelection.end());

	do {
		poseThumbSelection.pop_back();
	} while (poseThumbSelection.size() > 3);
}