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
	wasUserTrackedLastFrame = isUserTracked;
}

void KinectManager::draw()
{
	if (isDrawBodyIndex) 
		drawBodyIndex();

	//int skelCount = this->kinect.getBodySource()->getBodies().size();
	//printf("skelCount = %i \n", skelCount);
	//for (int i = 0; i < skelCount; i++)
	//{
	//	ofxKinectForWindows2::Data::Body body = this->kinect.getBodySource()->getBodies()[0];
	// 
	//	if (body.tracked)
	//		cout << i << " is tracked" << endl;
	//  
	//  
	// }


	//this->kinect.getBodySource()->drawBodies(0,0,640,480);
	//this->kinect.getBodySource()->drawBodies();

	if (isDrawSkeletons)
	{
		ofPushMatrix();
		ofScale(1, 1);
		drawSkeletons();
		ofPopMatrix();
	}
}


void KinectManager::drawBodyIndex()
{
	this->kinect.getBodyIndexSource()->draw(0, 0, 640 * 0.5, 480 * 0.5);
}


void KinectManager::drawSkeletons()
{
	this->kinect.getBodySource()->drawProjected(0, 0, 640 * 0.5, 480 * 0.5);
}


vector<Body> KinectManager::getSkeletonData()
{
	vector<Body> bodies = kinect.getBodySource()->getBodies();
	vector<Body> activeBodies;
	for (int i = 0; i < kinect.getBodySource()->getBodies().size(); i++)
	{
		if (bodies[i].tracked)
			activeBodies.push_back(bodies[i]);
	}
	vector<Body> activeBodiesWithinBounds;
	for (int i = 0; i < activeBodies.size(); i++)
	{
		ofVec3f spineBasePos = activeBodies[i].joints[JointType_SpineBase].getPosition();
		if (spineBasePos.x > boundsXMin && spineBasePos.x < boundsXMax && spineBasePos.z > boundsZMin && spineBasePos.z < boundsZMax)
		{
			activeBodiesWithinBounds.push_back(activeBodies[i]);
			isUserTracked = true;
			break;
		}
	}

	if (activeBodiesWithinBounds.size() == 0)
		isUserTracked = false;

	return activeBodiesWithinBounds;
}