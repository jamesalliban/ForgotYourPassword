#include "SceneManager.h"

void SceneManager::setup()
{
	srcW = 512;
	srcH = 424;
	depthFbo.allocate(srcW, srcH);

	videoFbo[0].allocate(srcW, srcH);
	videoFbo[1].allocate(srcW, srcH);
	currentVidFbo = 0;
	
	userFbo[0].allocate(srcW, srcH);
	userFbo[1].allocate(srcW, srcH); 
	currentUserFbo = 0;

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

		// check to see if the video has completed
		if (player.getCurrentFrame() == player.getTotalNumFrames() - 1)
		{
			player.stop();
			isPlayingSequence = false;
			isUserVisible = true;
			isDancerVisible = false;
		}
		
		// blur the video horizontally
		currentVidFbo = 1 - currentVidFbo;
		videoFbo[currentVidFbo].begin();
		blurShaderH.begin();
		blurShaderH.setUniform1f("blurAmount", dancerBlurAmount);
		blurShaderH.setUniform1i("numSamples", dancerBlurSamples);
		blurShaderH.setUniform2f("resolution", videoFbo[currentVidFbo].getWidth(), videoFbo[currentVidFbo].getHeight());
		player.draw(0,0);
		blurShaderH.end();
		videoFbo[currentVidFbo].end();
		
		// blur the video vertically
		currentVidFbo = 1 - currentVidFbo;
		videoFbo[currentVidFbo].begin();
		blurShaderV.begin();
		blurShaderV.setUniform1f("blurAmount", dancerBlurAmount);
		blurShaderV.setUniform1i("numSamples", dancerBlurSamples);
		blurShaderV.setUniform2f("resolution", videoFbo[currentVidFbo].getWidth(), videoFbo[currentVidFbo].getHeight());
		videoFbo[1 - currentVidFbo].draw(0,0);
		blurShaderV.end();
		videoFbo[currentVidFbo].end();
		
		// pass the blurred image to the dancer silhouette and update
		ofPixels pix;
		videoFbo[currentVidFbo].readToPixels(pix);
		videoImg.setFromPixels(pix);
		dancerSilhouette.update(videoImg);

		if (player.getCurrentFrame() == player.getTotalNumFrames())
			isPlayingSequence = false;
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


	// blur the user horizontally
	currentUserFbo = 1 - currentUserFbo;
	userFbo[currentUserFbo].begin();
	blurShaderH.begin();
	blurShaderH.setUniform1f("blurAmount", userBlurAmount);
	blurShaderH.setUniform1i("numSamples", userBlurSamples);
	blurShaderH.setUniform2f("resolution", userFbo[currentVidFbo].getWidth(), userFbo[currentVidFbo].getHeight());
	depthFbo.draw(0,0);
	blurShaderH.end();
	userFbo[currentUserFbo].end();
	
	// blur the user vertically
	currentUserFbo = 1 - currentUserFbo;
	userFbo[currentUserFbo].begin();
	blurShaderH.begin();
	blurShaderH.setUniform1f("blurAmount", userBlurAmount);
	blurShaderH.setUniform1i("numSamples", userBlurSamples);
	blurShaderH.setUniform2f("resolution", userFbo[currentVidFbo].getWidth(), userFbo[currentVidFbo].getHeight());
	userFbo[1 - currentUserFbo].draw(0,0);
	blurShaderH.end();
	userFbo[currentUserFbo].end();

	// pass blurred user image to user silhouette and update
	ofPixels pix;
	userFbo[currentVidFbo].readToPixels(pix);
	userImg.setFromPixels(pix);
	userSilhouette.update(userImg);
}


void SceneManager::draw(Depth & depth)
{
	if (player.isLoaded() && isPlayingSequence)
	{
		if (isDancerVisible)
			dancerSilhouette.draw();
	}

	if (isUserVisible)
		userSilhouette.draw();
}


void SceneManager::drawDebug(Depth & depth)
{
	if (player.isLoaded() && isPlayingSequence)
    {
		
		if (isDancerSilhouetteDebugVisible)
		{
			// Draw the video frame
			ofSetColor(255, 255, 255);
			ofPushMatrix();
			ofTranslate(0, ofGetHeight() - player.getHeight() * vidScale);
			ofScale(vidScale, vidScale);
			player.draw(0, 0);
			ofPopMatrix();

			// draw blurred
			ofPushMatrix();
			ofTranslate(srcW * vidScale, ofGetHeight() - player.getHeight() * vidScale);
			ofScale(vidScale, vidScale);
			videoFbo[currentVidFbo].draw(0, 0);
			ofPopMatrix();

			// draw contour
			ofPushStyle();
			ofSetColor(255, 0, 0);
			ofPushMatrix();
			ofTranslate(srcW * 2 * vidScale, ofGetHeight() - player.getHeight() * vidScale);
			ofScale(vidScale, vidScale);
			dancerSilhouette.drawContour();
			ofPopMatrix();
			ofPopStyle();
		}
		if (isUserSilhouetteDebugVisible)
		{
			// draw blurred
			ofPushMatrix();
			ofTranslate(srcW * vidScale, ofGetHeight() - player.getHeight() * vidScale * 2);
			ofScale(vidScale, vidScale);
			userFbo[currentVidFbo].draw(0, 0);
			ofPopMatrix();

			// draw contour
			ofPushStyle();
			ofSetColor(255, 0, 0);
			ofPushMatrix();
			ofTranslate(srcW * 2 * vidScale, ofGetHeight() - player.getHeight() * vidScale * 2);
			ofScale(vidScale, vidScale);
			userSilhouette.drawContour();
			ofPopMatrix();
			ofPopStyle();
		}
    }

	if (isDrawDepth)
		drawDepth(depth, 0, 480 * 0.5);

	if (isDrawClippedDepth)
	{
		ofPushMatrix();
		ofTranslate(0, ofGetHeight() - player.getHeight() * vidScale * 2);
		ofScale(vidScale, vidScale);
		drawClippedDepth(0, 0);
		ofPopMatrix();
	}

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
	depthFbo.draw(x, y, depthFbo.getWidth(), depthFbo.getHeight());
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

		isUserVisible = false;
		isDancerVisible = true;
	}
}


void SceneManager::stopVideo()
{	
	player.stop();
	isPlayingSequence = false;
	isUserVisible = true;
	isDancerVisible = false;
}


void SceneManager::pauseVideo()
{	
	if (isPlayingSequence)
	{
		player.setPaused(true);
	}
}


void SceneManager::resumeVideo()
{	
	if (isPlayingSequence)
	{
		player.setPaused(false);
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