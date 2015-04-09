#include "KinectManager.h"

void KinectManager::setup()
{
	kinect.open();
	kinect.initDepthSource();
	kinect.initColorSource();
	kinect.initInfraredSource();
	kinect.initBodyIndexSource();
	kinect.initBodySource();
}

void KinectManager::update()
{
	this->kinect.update();

	
}

void KinectManager::draw()
{
	//this->kinect.getDepthSource()->draw(0,0,640,480); // note that the depth texture is RAW so may appear dark
	//this->kinect.getColorSource()->draw(640,0,640,480);
	//this->kinect.getInfraredSource()->draw(0,480,640,480);

	if (isDrawBodyIndex) 
		drawBodyIndex();

	//int skelCount = this->kinect.getBodySource()->getBodies().size();
	//printf("skelCount = %i \n", skelCount);
	//for (int i = 0; i < skelCount; i++)
	//{
	//	ofxKinectForWindows2::Data::Body body = this->kinect.getBodySource()->getBodies()[0];

	//	if (body.tracked)
	//		cout << i << " is tracked" << endl;
	//	
	//}

	
	//this->kinect.getBodySource()->drawBodies(0,0,640,480);
	//this->kinect.getBodySource()->drawBodies();

	if (isDrawSkeletons)
		drawSkeletons();
}


void KinectManager::drawBodyIndex()
{
	this->kinect.getBodyIndexSource()->draw(0,0,640,480);
}


void KinectManager::drawSkeletons()
{
	this->kinect.getBodySource()->drawProjected(0,0,640,480);
}



vector<Body> KinectManager::getSkeletonData()
{
	return kinect.getBodySource()->getBodies();
}