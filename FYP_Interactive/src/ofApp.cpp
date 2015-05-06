#include "ofApp.h"

/*
TODO: 

Having trouble with shader - Sampler2DRect, extension etc. Check output.


- Graphical silhouettes:
	- set up shader to blur the images - this should reduce noise and combine separate blobs
	- Add Resample stuff to gui.
	- Play
		- Motion reduces resample count
		
- Transition
	- Match x, y and height of bounding box
	- Try morphing between points - match resample count
	- Try morphing back after only if a user is in place. Otherwise fade out.

- Display poses to audience:
	- When user first enters, show a few random stances. Continue every 10 seconds until one is recognised.
	- Add copy above - "Unlock your avatar"
	- Add copy below - "Enact a pose"

	
- Audio
	- Each time a sequence is started, select a random part of the sound file and fade in/out

- Limit the interaction to a single person standing in a specific zone

- Test on projector






NICE TO HAVE:

*/


void ofApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	//ofSetLogLevel(OF_LOG_SILENT);
	ofSetWindowShape(1800, 1000);
	ofSetFrameRate(30);
	ofSetBackgroundAuto(false);
	ofSetFullscreen(true);
    ofDisableArbTex();

	kinectManager.setup();
	poseManager.setup();
	ofAddListener(poseManager.poseRecognisedEvent, this, &ofApp::poseRecognised);
	sceneManager.setup();
	int danceSequenceSize = sceneManager.getSequenceSize();
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
		ofDrawBitmapString(debugStr, 640 * 0.5 + 10, 600);
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
	if (key == 'S')
	{
		//poseManager.savePose();
		isNewPoseHack = true;
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