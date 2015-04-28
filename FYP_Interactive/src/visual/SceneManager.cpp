#include "SceneManager.h"

void SceneManager::setup()
{
	depthFbo.allocate(512, 424);
	videoFbo.allocate(512, 424);
	
	screenRecorder.setup();
	screenRecorder.setImageFormat("tiff");
	loadShaders();

	isPlayingSequence = false;

	contourFinder.setMinAreaRadius(10);
	contourFinder.setMaxAreaRadius(150);
}

void SceneManager::update(Depth & depth)
{
	if (isPlayingSequence)
	{
		player.update();
		if (player.getCurrentFrame() == player.getTotalNumFrames() - 1)
		{
			player.stop();
			isPlayingSequence = false;
		}
		contourFinder.setThreshold(videoSilhouetteThreshold);
	}


	if (isLiveClipping)
	{
		depthFbo.begin();
		depthShader.begin();
		depthShader.setUniform1i("isClipping", (isClipping) ? 1 : 0);
		depthShader.setUniform2f("resolution", 512, 424);
		depthShader.setUniform1f("nearClip", liveNearClip);
		depthShader.setUniform1f("farClip", liveFarClip);
		depthShader.setUniform1f("nearFloorClip", liveNearFloorClip);
		depthShader.setUniform1f("farFloorClip", liveFarFloorClip);
		depthShader.setUniform1f("nearCeilingClip", liveNearCeilingClip);
		depthShader.setUniform1f("farCeilingClip", liveFarCeilingClip);
		depthShader.setUniform1f("nearLeftClip", liveNearLeftClip);
		depthShader.setUniform1f("farLeftClip", liveFarLeftClip);
		depthShader.setUniform1f("nearRightClip", liveNearRightClip);
		depthShader.setUniform1f("farRightClip", liveFarRightClip);
		depth.draw(0, 0, 512, 424);
		depthShader.end();
		depthFbo.end();
	}
	else
	{
		depthFbo.begin();
		depthShader.begin();
		depthShader.setUniform1i("isClipping", (isClipping) ? 1 : 0);
		depthShader.setUniform2f("resolution", 512, 424);
		depthShader.setUniform1f("nearClip", recNearClip);
		depthShader.setUniform1f("farClip", recFarClip);
		depthShader.setUniform1f("nearFloorClip", recNearFloorClip);
		depthShader.setUniform1f("farFloorClip", recFarFloorClip);
		depthShader.setUniform1f("nearCeilingClip", recNearCeilingClip);
		depthShader.setUniform1f("farCeilingClip", recFarCeilingClip);
		depthShader.setUniform1f("nearLeftClip", recNearLeftClip);
		depthShader.setUniform1f("farLeftClip", recFarLeftClip);
		depthShader.setUniform1f("nearRightClip", recNearRightClip);
		depthShader.setUniform1f("farRightClip", recFarRightClip);
		depth.draw(0, 0, 512, 424);
		depthShader.end();
		depthFbo.end();
	}
}


void SceneManager::draw(Depth & depth)
{
}


void SceneManager::drawDebug(Depth & depth)
{
	if (player.isLoaded() && isPlayingSequence)
    {
		videoFbo.begin();
		ofClear(1.0,1.0,1.0,1.0);
		player.draw(0,0);
		videoFbo.end();
		
		ofPixels pix;
		videoFbo.readToPixels(pix);
		ofImage img;
		img.setFromPixels(pix);
		contourFinder.findContours(img);

		img.draw(640 + 512, ofGetHeight() - player.getHeight());

        // Draw the video frame
        ofSetColor(255, 255, 255);
		player.draw(640, ofGetHeight() - player.getHeight());

		if (player.getCurrentFrame() == player.getTotalNumFrames())
			isPlayingSequence = false;

		ofPushMatrix();
		ofTranslate(640 + 512, ofGetHeight() - player.getHeight());
		contourFinder.draw();
		ofPopMatrix();
    }

	if (isDrawDepth)
		drawDepth(depth, 0, 480);

	if (isDrawClippedDepth)
		drawClippedDepth(0, 480 + 424);
	
	ofPushMatrix();
	ofTranslate(640, 0);
	ofScale(2.0, 2.0);
	depthFbo.draw(0, 0);
	ofPopMatrix();
   


    if (screenRecorder.getIsRecording())
	{
		fastFboReader.readToPixels(depthFbo, pix);
		screenRecorder.addFrame(pix);
	}
	ofPushStyle();
	ofSetColor(0);
	ofPushMatrix();
	ofTranslate(ofGetWidth() - 100, 20);
	screenRecorder.draw();
	ofPopMatrix();
	ofPopStyle();
}


void SceneManager::drawDepth(Depth & depth, int x, int y)
{
	depthShader.begin();
	depthShader.setUniform1i("isClipping", 0);
	depth.draw(x, y);
	depthShader.end();
}


void SceneManager::drawClippedDepth(int x, int y)
{
	depthFbo.draw(x, y);
}


void SceneManager::loadShaders()
{
	depthShader.load("shaders/DepthShader");
}



void SceneManager::toggleRecording()
{
	if (screenRecorder.getIsRecording())
		screenRecorder.stopRecording();
	else
		screenRecorder.startRecording();
}


void SceneManager::playVideo(int sequenceID)
{
	if (!isPlayingSequence)
	{
		if (sequenceID == 5)
			sequenceID = 4;
		string sId = ofToString(sequenceID);
		if (sId.size() < 2)
			sId = '0' + sId;

		player.loadMovie("movies/FYP_Sequence_" + sId + ".mov");
		player.play();
	
		cout << "playVideo " << "movies/FYP_Sequence_" + sId + ".mov" << endl;

		isPlayingSequence = true;
	}
}


void SceneManager::exit()
{
	screenRecorder.exit();
}