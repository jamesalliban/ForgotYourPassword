#include "Instructions.h"

void Instructions::setup(int _srcW, int _srcH)
{
	srcW = _srcW;
	srcH = _srcH;

	isActive = false;
	isIntro = false;
	isOutro = false;
	
	//hideInstructionsTimer.setup(100, "HIDE");
	//hideInstructionsTimer.setup(6600, "HIDE");
	//hideInstructionsTimer.setup(7000, "HIDE");
	//hideInstructionsTimer.setup(5000, "HIDE");
	//hideInstructionsTimer.setup(1000, "HIDE");
	ofAddListener(hideInstructionsTimer.TIMER_COMPLETE, this, &Instructions::doOutro);
	
	//showInstructionsTimer.setup(1000, "SHOW");
	ofAddListener(showInstructionsTimer.TIMER_COMPLETE, this, &Instructions::doIntro);

	startShowTimer(1000);
}


void Instructions::loadPoses()
{
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
	showInstructionsTimer.update();
	hideInstructionsTimer.update();

	if (isIntro)
	{
		currentAlpha += introSpeed;
		if (currentAlpha > 255)
		{
			isIntro = false;
			currentAlpha = 255;
		}
	}
	else if (isOutro)
	{
		currentAlpha -= outroSpeed;
		if (currentAlpha < 0)
		{
			isOutro = false;
			currentAlpha = 0;
			isActive = false;
		}
	}
}


void Instructions::draw()
{
	if (!isActive)
		return;

	ofPushStyle();
	for (int i = 0; i < poseThumbSelection.size(); i++)
	{
		ofPushMatrix();
		int x = ofMap(i, 0, 2, (ofGetWidth() * 0.5) - poseSelectionSpacing, (ofGetWidth() * 0.5) + poseSelectionSpacing);
		ofTranslate(x, poseSelectionYPos);
		ofScale(poseSelectionScale, poseSelectionScale);

		ofSetColor(255, currentAlpha);
		for (int j = 0; j < poseThumbSelection[i]->shapes.size(); j++) 
		{
			poseThumbSelection[i]->shapesOffset = poseThumbSelection[i]->shapes;
			vector<ofPoint>& vertices = poseThumbSelection[i]->shapes[j].getVertices();
			vector<ofPoint>& verticesOffset = poseThumbSelection[i]->shapesOffset[j].getVertices();
			ofBeginShape();
			for (int k = 0; k < vertices.size(); k++) 
			{
				//ofPoint pnt = vertices[k];
				verticesOffset[k].x = vertices[k].x + ofRandomf() * randOffset;
				verticesOffset[k].y = vertices[k].y + ofRandomf() * randOffset;
				ofVertex(verticesOffset[k]);
			}
			ofEndShape();
		}
		ofSetColor(0, currentAlpha);
		for (int j = 0; j < poseThumbSelection[i]->holes.size(); j++) 
		{
			poseThumbSelection[i]->holesOffset = poseThumbSelection[i]->holes;
			vector<ofPoint>& vertices = poseThumbSelection[i]->holes[j].getVertices();
			vector<ofPoint>& verticesOffset = poseThumbSelection[i]->holesOffset[j].getVertices();
			ofBeginShape();
			for (int k = 0; k < vertices.size(); k++) 
			{
				verticesOffset[k].x = vertices[k].x + ofRandomf() * randOffset;
				verticesOffset[k].y = vertices[k].y + ofRandomf() * randOffset;
				ofVertex(verticesOffset[k]);
			}
			ofEndShape();
		}

		ofSetColor(0, currentAlpha);
		for (int j = 0; j < poseThumbSelection[i]->shapes.size(); j++) 
			poseThumbSelection[i]->shapesOffset[j].draw();

		ofPopMatrix();
	}

	ofSetColor(255, currentAlpha);
	
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


void Instructions::startShowTimer(int millis)
{
	showInstructionsTimer.stop();
	showInstructionsTimer.setup(millis, "SHOW");
	showInstructionsTimer.start(false);

}


void Instructions::doIntro(int &args)
{
	isActive = true;
	isIntro = true;
	selectNewPoses();
	hideInstructionsTimer.setup(displayTime, "HIDE");
	hideInstructionsTimer.start(false);

	showInstructionsTimer.setup(showLoopDelay, "SHOW");
	showInstructionsTimer.start(false);
}


void Instructions::doOutro(int &args)
{
	isOutro = true;
}


void Instructions::stopAll()
{
	hideInstructionsTimer.stop();
	showInstructionsTimer.stop();

	if (isIntro)
	{
		isIntro = false;
		int tempHack = 1;
		doOutro(tempHack);
	}
	else if (isOutro)
	{
		// continue
	}
	else if (isActive)
	{
		int tempHack = 1;
		doOutro(tempHack);
	}
}