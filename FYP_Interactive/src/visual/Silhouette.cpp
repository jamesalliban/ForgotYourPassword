
#include "Silhouette.h"

void Silhouette::setup(int _srcW, int _srcH)
{
	srcW = _srcW;
	srcH = _srcH;
	contourFinder.setMinAreaRadius(10);
	contourFinder.setMaxAreaRadius(150);
}


void Silhouette::update(ofImage & img)
{
	contourFinder.setThreshold(imageContourThreshold);
	contourFinder.findContours(img);
}


void Silhouette::draw()
{
	vector<ofPolyline> polylines;

	for (int i = 0; i < contourFinder.getContours().size(); i++)
	{
		ofPolyline polyline;
		for (int j = 0; j < contourFinder.getContour(i).size(); j++)
		{
			ofPoint pnt = toOf(contourFinder.getContour(i)[j]);
			pnt.x -= srcW * 0.5;
			pnt.y -= srcH * 0.5;
			polyline.addVertex(pnt);
		}
		polyline = polyline.getResampledByCount(resampleAmount);
		polylines.push_back(polyline);
	}

	ofPushMatrix();
	ofTranslate(position.x * ofGetWidth(), position.y * ofGetHeight());
	ofScale(scale, scale);
	for (int i = 0; i < polylines.size(); i++)
	{
		ofBeginShape();
		vector<ofPoint>& vertices = polylines[i].getVertices();
		for(int j = 0; j < vertices.size(); j++) 
		{
			ofVertex(vertices[j]);
		}
		ofEndShape();
		//polylines[i].draw();
	}
	ofPopMatrix();
}


void Silhouette::drawContour()
{
	contourFinder.draw();
}