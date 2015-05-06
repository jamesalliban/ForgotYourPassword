#include "SceneManager.h"

void SceneManager::setup()
{
	srcW = 512;
	srcH = 424;
	depthFbo.allocate(srcW, srcH);

	videoFbo[0].allocate(srcW, srcH);
	videoFbo[1].allocate(srcW, srcH);
	currentVidFbo = 0;
	
	screenRecorder.setup();
	screenRecorder.setImageFormat("tiff");
	loadShaders();

	isPlayingSequence = false;
	
	userSilhouette.setup(srcW, srcH);
	dancerSilhouette.setup(srcW, srcH);	
	
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
		
		currentVidFbo = 1 - currentVidFbo;
		videoFbo[currentVidFbo].begin();
		//ofClear(0.0, 0.0, 0.0, 1.0);
		blurShaderH.begin();
		blurShaderH.setUniform1f("blurAmount", dancerBlurAmount);
		blurShaderH.setUniform1i("numSamples", dancerBlurSamples);
		blurShaderH.setUniform2f("resolution", videoFbo[currentVidFbo].getWidth(), videoFbo[currentVidFbo].getHeight());
		player.draw(0,0);
		blurShaderH.end();
		videoFbo[currentVidFbo].end();
		
		currentVidFbo = 1 - currentVidFbo;
		videoFbo[currentVidFbo].begin();
		//ofClear(0.0, 0.0, 0.0, 1.0);
		blurShaderV.begin();
		blurShaderV.setUniform1f("blurAmount", dancerBlurAmount);
		blurShaderV.setUniform1i("numSamples", dancerBlurSamples);
		blurShaderV.setUniform2f("resolution", videoFbo[currentVidFbo].getWidth(), videoFbo[currentVidFbo].getHeight());
		videoFbo[1 - currentVidFbo].draw(0,0);
		blurShaderV.end();
		videoFbo[currentVidFbo].end();
		

		ofPixels pix;
		videoFbo[currentVidFbo].readToPixels(pix);
		videoImg.setFromPixels(pix);
		dancerSilhouette.update(videoImg);
	}


	if (isLiveClipping)
	{
		depthFbo.begin();
		depthShader.begin();
		depthShader.setUniform1i("isClipping", (isClipping) ? 1 : 0);
		depthShader.setUniform2f("resolution", srcW, srcH);
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
		depth.draw(0, 0, srcW, srcH);
		depthShader.end();
		depthFbo.end();
	}
	else
	{
		depthFbo.begin();
		depthShader.begin();
		depthShader.setUniform1i("isClipping", (isClipping) ? 1 : 0);
		depthShader.setUniform2f("resolution", srcW, srcH);
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
		depth.draw(0, 0, srcW, srcH);
		depthShader.end();
		depthFbo.end();
	}
}


void SceneManager::draw(Depth & depth)
{
	if (player.isLoaded() && isPlayingSequence)
	{
		dancerSilhouette.draw();
	}
}


void SceneManager::drawDebug(Depth & depth)
{
	if (player.isLoaded() && isPlayingSequence)
    {
		float vidScale = 0.5;
		
        // Draw the video frame
        ofSetColor(255, 255, 255);
		ofPushMatrix();
		ofTranslate(0, ofGetHeight() - player.getHeight() * vidScale);
		ofScale(vidScale, vidScale);
		player.draw(0, 0);
		ofPopMatrix();

		// draw first blur
		ofPushMatrix();
		ofTranslate(srcW * vidScale, ofGetHeight() - player.getHeight() * vidScale);
		ofScale(vidScale, vidScale);
		videoFbo[1 - currentVidFbo].draw(0, 0);
		ofPopMatrix();

		// draw second blur
		ofPushMatrix();
		ofTranslate(srcW * 2 * vidScale, ofGetHeight() - player.getHeight() * vidScale);
		ofScale(vidScale, vidScale);
		videoFbo[currentVidFbo].draw(0, 0);
		ofPopMatrix();

		if (player.getCurrentFrame() == player.getTotalNumFrames())
			isPlayingSequence = false;

		// draw contour
		ofPushMatrix();
		ofTranslate(srcW * 3 * vidScale, ofGetHeight() - player.getHeight() * vidScale);
		ofScale(vidScale, vidScale);
		dancerSilhouette.drawContour();
		ofPopMatrix();
    }

	if (isDrawDepth)
		drawDepth(depth, 0, 480 * 0.5);

	if (isDrawClippedDepth)
		drawClippedDepth(0, 480 * 0.5 + srcH * 0.6);
	
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
	blurShaderH.load("shaders/BlurShaderH");
	blurShaderV.load("shaders/BlurShaderV");
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


int SceneManager::getSequenceSize()
{
	ofDirectory dir;
	dir.listDir("movies");
	return dir.getFiles().size();
}


void SceneManager::exit()
{
	screenRecorder.exit();
}