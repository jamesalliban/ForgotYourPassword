#include "SceneManager.h"

void SceneManager::setup()
{
	depthFbo.allocate(512, 424);
	
	screenRecorder.setup();
	screenRecorder.setImageFormat("tiff");
	loadShaders();

	isPlayingSequence = false;
}

void SceneManager::update(Depth & depth)
{
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
	if (isDrawDepth)
		drawDepth(depth);

	if (isDrawClippedDepth)
		drawClippedDepth();
	
	ofPushMatrix();
	ofTranslate(640, 0);
	ofScale(3.0, 3.0);
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

	if (isPlayingSequence)
	{
		if (imgSequenceLoader.q.size() > 0)
		{
			//cout << "drawing image - imgSequenceLoader.q.size() = " << imgSequenceLoader.q.size() << endl;
			currentLoadedImg = imgSequenceLoader.getImage();
			currentLoadedImg.setUseTexture(true);
			currentLoadedImg.update();
			currentLoadedImg.draw(200, 200);

		}

		ofPushStyle();
		ofSetColor(0);
		ofDrawBitmapString("q size = " + ofToString(imgSequenceLoader.q.size()), ofGetWidth() - 400, 300);
		ofPopStyle();
	}
}


void SceneManager::drawDepth(Depth & depth)
{
	depthShader.begin();
	depthShader.setUniform1i("isClipping", 0);
	depth.draw(0, 480);
	depthShader.end();
}


void SceneManager::drawClippedDepth()
{
	depthFbo.draw(0, 480 * 2.0);
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


void SceneManager::playImageSequence(int sequenceID)
{
	isPlayingSequence = true;
	//imgSequenceLoader.setLoadPath("recordings/recording_2015-04-02_14.17.04");
	imgSequenceLoader.setLoadPath("image_sequences/sequence_" + ofToString(sequenceID));
	imgSequenceLoader.startLoading();
}


void SceneManager::exit()
{
	screenRecorder.exit();
	imgSequenceLoader.waitForThread();
}