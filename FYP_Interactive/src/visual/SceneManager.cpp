#include "SceneManager.h"

void SceneManager::setup()
{
	depthFbo.allocate(512, 424);
	videoFbo.allocate(512, 424);
	
	screenRecorder.setup();
	screenRecorder.setImageFormat("tiff");
	loadShaders();

	isPlayingSequence = false;
	
	userSilhouette.setup();
	dancerSilhouette.setup();
}

void SceneManager::update(Depth & depth)
{
	if (player.isLoaded() && isPlayingSequence)
	{
		player.update();
		if (player.getCurrentFrame() == player.getTotalNumFrames() - 1)
		{
			player.stop();
			isPlayingSequence = false;
		}

		videoFbo.begin();
		ofClear(1.0,1.0,1.0,1.0);
		player.draw(0,0);
		videoFbo.end();

		ofPixels pix;
		videoFbo.readToPixels(pix);
		videoImg.setFromPixels(pix);
		dancerSilhouette.update(videoImg);
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
	if (player.isLoaded() && isPlayingSequence)
	{
		ofPushMatrix();
		ofTranslate(400, 200);
		ofScale(2, 2);
		dancerSilhouette.draw();
		ofPopMatrix();
	}
}


void SceneManager::drawDebug(Depth & depth)
{
	if (player.isLoaded() && isPlayingSequence)
    {
		videoImg.draw(512, ofGetHeight() - player.getHeight());

        // Draw the video frame
        ofSetColor(255, 255, 255);
		player.draw(0, ofGetHeight() - player.getHeight());

		if (player.getCurrentFrame() == player.getTotalNumFrames())
			isPlayingSequence = false;

		ofPushMatrix();
		ofTranslate(512, ofGetHeight() - player.getHeight());
		dancerSilhouette.drawContour();
		ofPopMatrix();
    }

	if (isDrawDepth)
		drawDepth(depth, 0, 480 * 0.5);

	if (isDrawClippedDepth)
		drawClippedDepth(0, 480 * 0.5 + 424 * 0.6);
	
	if (isDrawLargeClippedDepth)
	{
		ofPushMatrix();
		ofTranslate(640 * 0.5, 0);
		ofScale(largeClippingScale, largeClippingScale);
		depthFbo.draw(0, 0);
		ofPopMatrix();
	}
   

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
	depth.draw(x, y, depth.getWidth() * 0.6, depth.getHeight() * 0.6);
	depthShader.end();
}


void SceneManager::drawClippedDepth(int x, int y)
{
	depthFbo.draw(x, y, depthFbo.getWidth() * 0.6, depthFbo.getHeight() * 0.6);
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