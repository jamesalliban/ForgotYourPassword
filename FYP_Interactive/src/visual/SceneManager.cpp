#include "SceneManager.h"

void SceneManager::setup()
{
	srcW = 512;
	srcH = 424;

	ofFbo::Settings settings;
	settings.width = srcW;
	settings.height = srcH;
	settings.internalformat = GL_RGB;

	depthFbo.allocate(settings);

	dancerFbo[0].allocate(settings);
	dancerFbo[1].allocate(settings);
	currentVidFbo = 0;
	userFbo[0].allocate(settings);
	userFbo[1].allocate(settings); 
	currentUserFbo = 0;

	instructions.setup(srcW, srcH);

	screenRecorder.setup();
	screenRecorder.setImageFormat("tiff");
	loadShaders();

	isPlayingSequence = false;
	isUserSilhouetteDebugVisible = true;
	
	userSilhouette.setup(srcW, srcH);
	dancerSilhouette.setup(srcW, srcH);	
	
	ofAddListener(dancerSilhouette.transitionCompleteEvent, this, &SceneManager::transitionComplete);

	isStartingVideo = false;
	
	dancerSilhouette.colour[0] = &silhouetteCol[0];
	dancerSilhouette.colour[1] = &silhouetteCol[1];
	dancerSilhouette.colour[2] = &silhouetteCol[2];
	userSilhouette.colour[0] = &silhouetteCol[0];
	userSilhouette.colour[1] = &silhouetteCol[1];
	userSilhouette.colour[2] = &silhouetteCol[2];
	
	dancerSilhouette.holeColour[0] = &bgCol[0];
	dancerSilhouette.holeColour[1] = &bgCol[1];
	dancerSilhouette.holeColour[2] = &bgCol[2];
	userSilhouette.holeColour[0] = &bgCol[0];
	userSilhouette.holeColour[1] = &bgCol[1];
	userSilhouette.holeColour[2] = &bgCol[2];
}


void SceneManager::setupInstructions()
{
	instructions.loadPoses();
}


void SceneManager::update(Depth & depth, bool isPaused)
{
	if (player.isLoaded() && isPlayingSequence && !isPaused)
	{
		player.update();

		dancerSilhouette.updateVideoProgress(player.getCurrentFrame(), player.getTotalNumFrames());

		// check to see if the video has completed
		if (player.getCurrentFrame() >= player.getTotalNumFrames() - 2)
		{
			cout << "stopping video" << endl;
			player.stop();
			isPlayingSequence = false;
			isUserVisible = true;
			isDancerVisible = false;

			userSilhouette.startIntroFade();
			
			float ddd;
			ofNotifyEvent(videoCompleteEvent, ddd);	

		}
		
		// blur the video horizontally
		currentVidFbo = 1 - currentVidFbo;
		dancerFbo[currentVidFbo].begin();
		blurShaderH.begin();
		blurShaderH.setUniform1f("blurAmount", dancerBlurAmount);
		blurShaderH.setUniform1i("numSamples", dancerBlurSamples);
		blurShaderH.setUniform2f("resolution", dancerFbo[currentVidFbo].getWidth(), dancerFbo[currentVidFbo].getHeight());
		player.draw(0,0);
		blurShaderH.end();
		dancerFbo[currentVidFbo].end();
		
		// blur the video vertically
		currentVidFbo = 1 - currentVidFbo;
		dancerFbo[currentVidFbo].begin();
		blurShaderV.begin();
		blurShaderV.setUniform1f("blurAmount", dancerBlurAmount);
		blurShaderV.setUniform1i("numSamples", dancerBlurSamples);
		blurShaderV.setUniform2f("resolution", dancerFbo[currentVidFbo].getWidth(), dancerFbo[currentVidFbo].getHeight());
		dancerFbo[1 - currentVidFbo].draw(0,0);
		blurShaderV.end();
		dancerFbo[currentVidFbo].end();
		
		// pass the blurred image to the dancer silhouette and update
		
		dancerFbo[currentVidFbo].readToPixels(dancerPix);
		//dancerImg.setFromPixels(pix);
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
	userFbo[currentVidFbo].readToPixels(userPix);
	
	if (dancerPix.getWidth() > 0)
	{
		if (isStartingVideo)
			dancerSilhouette.update(userPix);
		else
			dancerSilhouette.update(dancerPix);
	}
	if (userPix.getWidth() > 0)
	{
		if (isStartingVideo)
		{
			isStartingVideo = false;
			dancerSilhouette.startAnimation(userPix);
		}
		//if (player.isPlaying)
		userSilhouette.update(userPix);
	}
	else
	{
		// just in case the user becomes unrecogniseable 1 frame after the pose
		// is recognised. Unlikely but possible
		if (isStartingVideo)
		{
			isStartingVideo = false;
			player.play();
			player.setFrame(player.getTotalNumFrames() - 3);
			dancerSilhouette.isIntro = false;
		}
	}
}


void SceneManager::drawBackground()
{
	ofPushStyle();
	ofSetColor(bgCol[0], bgCol[1], bgCol[2]);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
	ofPopStyle();

	instructions.update();
}


void SceneManager::draw(Depth & depth)
{
	if (player.isLoaded() && isPlayingSequence)
	{
		if (isDancerVisible)
		{
			dancerSilhouette.draw();
		}
	}

	if (isUserVisible)
		userSilhouette.draw();

	instructions.draw();
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
			ofTranslate(0, ofGetHeight() - srcH * vidScale);
			ofScale(vidScale, vidScale);
			player.draw(0, 0);
			ofPopMatrix();

			// draw blurred
			ofPushMatrix();
			ofTranslate(srcW * vidScale, ofGetHeight() - srcH * vidScale);
			ofScale(vidScale, vidScale);
			dancerFbo[currentVidFbo].draw(0, 0);
			ofPopMatrix();

			// draw CV Grey image
			ofPushMatrix();
			ofTranslate(srcW * 2 * vidScale, ofGetHeight() - srcH * vidScale);
			ofScale(vidScale, vidScale);
			dancerSilhouette.drawCvGreyImg();
			ofPopMatrix();

			// draw contour
			ofPushStyle();
			ofSetColor(255, 0, 0);
			ofPushMatrix();
			ofTranslate(srcW * 3 * vidScale, ofGetHeight() - srcH * vidScale);
			ofScale(vidScale, vidScale);
			dancerSilhouette.drawContour();
			ofPopMatrix();
			ofPopStyle();
		}
    }

	if (isUserSilhouetteDebugVisible)
	{
		// draw blurred
		ofPushMatrix();
		ofTranslate(srcW * vidScale, ofGetHeight() - srcH * vidScale * 2);
		ofScale(vidScale, vidScale);
		userFbo[currentVidFbo].draw(0, 0);
		ofPopMatrix();

		// draw CV Grey image
		ofPushMatrix();
		ofTranslate(srcW * 2 * vidScale, ofGetHeight() - srcH * vidScale * 2);
		ofScale(vidScale, vidScale);
		userSilhouette.drawCvGreyImg();
		ofPopMatrix();

		// draw contour
		ofPushStyle();
		ofSetColor(255, 0, 0);
		ofPushMatrix();
		ofTranslate(srcW * 3 * vidScale, ofGetHeight() - srcH * vidScale * 2);
		ofScale(vidScale, vidScale);
		userSilhouette.drawContour();
		ofPopMatrix();
		ofPopStyle();
	}

	if (isDrawDepth)
		drawDepth(depth, 0, 480 * 0.5);

	if (isDrawClippedDepth)
	{
		ofPushMatrix();
		ofTranslate(0, ofGetHeight() - srcH * vidScale * 2);
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
	string sId = ofToString(sequenceID);
	if (sId.size() < 2)
		sId = '0' + sId;
	
	player.stop();
	player.close();
	player.loadMovie("movies/FYP_Sequence_" + sId + ".mov");
	player.play();

	player.setFrame(2);
	player.update();
	
	player.setPaused(true);
	isStartingVideo = true;
	
	cout << "playVideo " << "movies/FYP_Sequence_" + sId + ".mov" << endl;

	isPlayingSequence = true;

	isUserVisible = false;
	isDancerVisible = true;

	dancerSilhouette.isIntro = true;

	instructions.stopAll();
}


void SceneManager::stopVideo()
{	
	player.stop();
	isPlayingSequence = false;
	isUserVisible = true;
	isDancerVisible = false;
	dancerSilhouette.stop();
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


void SceneManager::scrubVideo(int offset)
{
	if (isPlayingSequence)
	{
		int newFrame = player.getCurrentFrame() + offset;
		newFrame = ofClamp(newFrame, 0, player.getTotalNumFrames() - 1);
		player.setFrame(newFrame);
	}
}


int SceneManager::getSequenceSize()
{
	ofDirectory dir;
	dir.listDir("movies");
	return dir.getFiles().size();
}


void SceneManager::transitionComplete(float & f)
{
	player.setPaused(false);
}


void SceneManager::exit()
{
	screenRecorder.exit();
}