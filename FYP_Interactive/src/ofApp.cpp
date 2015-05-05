#include "ofApp.h"

/*
TODO: 
- Unlock dance sequences with poses:
	- Link poses to dance sequences.
	- Limit the interaction to people standing in a specifit zone
- Test on projector
- Graphical silhouettes:
	- set up shader to blur the images - this should reduce noise and combine separate blobs
	- Add Resample stuff to gui.
	- Play
		- Motion reduces resample count
- Sort out dance videos
    - Batch convert all images to proper tiffs using FastStone - - IS THIS STILL REQUIRED?????
	- Create hap videos in Premiere.

- Only track a single person in a specific location/zone.
- After a 30 seconds no recognised stance, show a few random stances. Continue every 10 seconds until one is recognised

- Transition
	- Match x, y and height of bounding box
	- Try morphing between points - match resample count


NICE TO HAVE:
- Try to take legs into account when comparing. Only compare legs when live is not in standing position



QUESTIONS
- Are we still having music?
- Landscape???

*/


void ofApp::setup()
{
	//ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetLogLevel(OF_LOG_SILENT);
	ofSetWindowShape(1800, 1000);
	ofSetFrameRate(30);
	ofSetBackgroundAuto(false);
	ofSetFullscreen(true);

	kinectManager.setup();
	poseManager.setup();
	ofAddListener(poseManager.poseRecognisedEvent, this, &ofApp::poseRecognised);
	sceneManager.setup();
	int danceSequenceSize = sceneManager.getSequenceSize();
	cout << danceSequenceSize << endl;
	gui.setup(danceSequenceSize, poseManager.getPoseImages());
	
	soundStream.setup(this, 0, 2, 44100, 256, 4);
	frameAtLastSoundEvent = 0;
	smoothAmplitude = 0;
	
	isNewPoseHack = false;
	isPaused = false;
}


void ofApp::update()
{
	if (!isPaused)
	{
		kinectManager.update();
		poseManager.update(kinectManager.getSkeletonData());
		sceneManager.update(*kinectManager.kinect.getDepthSource());
	}

	if (isNewPoseHack)
	{
		cout << "NEW POSE" << endl;
		isNewPoseHack = false;
		poseManager.recordNewPose(kinectManager.getSkeletonData());
	}
}


void ofApp::draw()
{
	ofPushStyle();
	ofSetColor(40);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
	ofPopStyle();
	//this->kinect.getLongExposureInfraredSource()->draw(0,0,640, 480);

	if (isDebugVisible)
		kinectManager.draw();
	
	sceneManager.draw(*kinectManager.kinect.getDepthSource());

	if (isDebugVisible)
	{
		sceneManager.drawDebug(*kinectManager.kinect.getDepthSource());
		poseManager.draw();

		string debugStr;
		debugStr += "----KINECT----\n"; 
		debugStr += "Num Skeletons" + ofToString(kinectManager.getSkeletonData().size()) + "\n"; 
		debugStr += "\n"; 
		debugStr += "----AUDIO----\n"; 
		debugStr += "smoothed amplitude = " + ofToString(smoothAmplitude) + "\n"; 

		ofPushStyle();
		ofSetColor(255);
		ofDrawBitmapString(debugStr, 640 * 0.5, 20);
		ofPopStyle();
	}
}


void ofApp::loadShaders()
{
	sceneManager.loadShaders();
}


void ofApp::poseRecognised(Pose & eventPose)
{
	sceneManager.playVideo(eventPose.id);
}


void ofApp::keyPressed(int key)
{
	if (key == 'l')
	{
		loadShaders();
	}
	if (key == 'p')
	{
		isPaused = !isPaused;
	}
	if (key == 'r')
	{
		sceneManager.toggleRecording();
	}
	if (key == 'f')
	{
		ofToggleFullscreen();
	}
	if (key == 's')
	{
		isNewPoseHack = true;
	}
	if (key == 'S')
	{
		poseManager.savePose();
	}
	if (key == 'd')
	{
		isDebugVisible = !isDebugVisible;
	}
	if (key == '0')
		sceneManager.playVideo(0);
	if (key == '1')
		sceneManager.playVideo(1);
	if (key == '2')
		sceneManager.playVideo(2);
	if (key == '3')
		sceneManager.playVideo(3);
	if (key == '4')
		sceneManager.playVideo(4);
}


//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels)
{
	//if (smoothAmplitude > minAmplitudeForEvent)
	//	cout << "smoothAmplitude = " << smoothAmplitude << endl;

	float amplitude = 0;
	for (int i = 0; i < bufferSize; i++)
		amplitude += abs(input[bufferSize]) * 10;
	amplitude /= bufferSize;
	
	smoothAmplitude = ofLerp(smoothAmplitude, amplitude, 0.1);

	if (smoothAmplitude > minAmplitudeForEvent && ofGetFrameNum() - frameAtLastSoundEvent > framesForSoundEvent && isSoundEventActive)
	{
		isNewPoseHack = true;
		frameAtLastSoundEvent = ofGetFrameNum();
	}
}




void ofApp::exit()
{
	sceneManager.exit();
}