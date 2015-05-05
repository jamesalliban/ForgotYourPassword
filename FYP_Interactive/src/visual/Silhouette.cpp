
#include "Silhouette.h"

void Silhouette::setup()
{
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
			polyline.addVertex(pnt);
			//polyline.addVertex(contourFinder.getContour(i)[j]);
			//polyline.addVertices(contourFinder.getContour(i));
		}
		polyline = polyline.getResampledByCount(100);
		polylines.push_back(polyline);
	}

	//vector<ofPath> paths;
	//for (int i = 0; i < polylines.size(); i++)
	//{
	//	ofPath path;
	//	path.set
	//	paths.push_back(path);
	//}


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
}


void Silhouette::drawContour()
{
	contourFinder.draw();
}