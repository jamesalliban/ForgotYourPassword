#include "ofApp.h"

/*
TODO:
		
- Transition
	- Try morphing between points - match resample count:
		- Stop flickr at start of morph where tweetTo is briefly shown.
		- Match x, y of bottom centre, and width of bounding box.
		- EaseInOut tween.
	- Outro should be a straight fade.

	
- MISC
	- Add start and end offsets for videos to gui - update the data being sent to soundManager
	- Instructions silhouettes should have holes instead of black shapes


*/


void ofApp::setup()
{
	//ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetLogLevel(OF_LOG_SILENT);
	ofSetWindowShape(1200, 1000);
	ofSetFrameRate(30);
	ofSetBackgroundAuto(false);
	ofSetFullscreen(false);
    ofDisableArbTex();
	ofEnableSmoothing();

	kinectManager.setup();
	poseManager.setup();
	ofAddListener(poseManager.poseRecognisedEvent, this, &ofApp::poseRecognised);
	sceneManager.setup();
	ofAddListener(sceneManager.videoCompleteEvent, this, &ofApp::videoComplete);
	int danceSequenceSize = sceneManager.getSequenceSize();
	soundManager.setup();

	gui.setup(danceSequenceSize, poseManager.getPoseImages());
	
	sceneManager.setupInstructions();

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
	}
	sceneManager.update(*kinectManager.kinect.getDepthSource(), isPaused);
	if (sceneManager.isPlayingSequence)
	{
		soundManager.update(sceneManager.player.getCurrentFrame(), sceneManager.player.getTotalNumFrames());
	}

	if (isNewPoseHack)
	{
		isNewPoseHack = false;
		poseManager.recordNewPose(kinectManager.getSkeletonData());
	}

	if (kinectManager.isUserTracked && !kinectManager.wasUserTrackedLastFrame)
		sceneManager.instructions.startShowTimer(startInstructionsNewUserDelay);
}


void ofApp::draw()
{
	sceneManager.drawBackground();

	if (isDebugVisible)
		kinectManager.draw();
	
	sceneManager.draw(*kinectManager.kinect.getDepthSource());

	if (isDebugVisible)
	{
		sceneManager.drawDebug(*kinectManager.kinect.getDepthSource());
		poseManager.draw();

		string debugStr;
		debugStr += "----KINECT----\n"; 
		debugStr += "Num Skeletons: " + ofToString(kinectManager.getSkeletonData().size()) + "\n"; 
		debugStr += "\n"; 
		debugStr += "----AUDIO----\n"; 
		debugStr += "Smooth amplitude: " + ofToString(float((int)(smoothAmplitude * 100)) / 100.0) + "\n"; 

		ofPushStyle();
		ofSetColor(255);
		ofDrawBitmapString(debugStr, ofGetWidth() - 200, ofGetHeight() - 180);
		ofPopStyle();
	}
}


void ofApp::loadShaders()
{
	sceneManager.loadShaders();
}


void ofApp::poseRecognised(Pose & eventPose)
{
	if (!sceneManager.isPlayingSequence) 
		sceneManager.playVideo(eventPose.id);
	if (!soundManager.isPlayingSound) 
		soundManager.startSound();
}

void ofApp::videoComplete(float & f)
{
	soundManager.stopSound();
	sceneManager.instructions.startShowTimer(startInstructionsVideoEndDelay);
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
		if (isPaused)
			sceneManager.pauseVideo();
		else
			sceneManager.resumeVideo();
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
	if (key == 's')
	{
		sceneManager.stopVideo();
		soundManager.stopSound();
		sceneManager.instructions.startShowTimer(startInstructionsVideoEndDelay);
		sceneManager.instructions.isVideoRunning = false;
	}
	if (key == 't')
	{
		poseManager.isTracking = !poseManager.isTracking;
	}
	if (key == 'd')
	{
		isDebugVisible = !isDebugVisible;
	}
	if (key == 'i')
	{
		sceneManager.setupInstructions();
	}
	if (key == OF_KEY_LEFT)
	{
		sceneManager.scrubVideo(-30);
	}
	if (key == OF_KEY_RIGHT)
	{
		sceneManager.scrubVideo(30);
	}
	if (key == 'I')
	{
		sceneManager.instructions.startShowTimer(1000);
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
	if (key == '5')
		sceneManager.playVideo(5);
	if (key == '6')
		sceneManager.playVideo(6);
	if (key == '7')
		sceneManager.playVideo(7);
	if (key == '8')
		sceneManager.playVideo(8);
	if (key == '9')
		sceneManager.playVideo(9);
}


//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels)
{
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