#include "GUI.h"
#include "ofApp.h"

void GUI::setup()
{
    app = (ofApp*)ofGetAppPtr();
    ofxUIManager::setup();
	
    
    addVariousGUI();
    addDepthBoundsGUI();
    addTrackingGUI();
	addSilhouetteGUI();
    addRecordingGUI();
    addDebugGUI();
    //addBackgroundGUI();
    addGUIDesignGUI();
    
    setGUIColour();
}


void GUI::addVariousGUI()
{
    string title = "VARIOUS";
    ofxUICanvas* gui = getNewGUI(title);
	//gui->setFont("GUI/faucet.ttf");
	gui->addLabel("KEY", OFX_UI_FONT_LARGE);
    gui->addLabel("SPACE - SHOW/HIDE GUI", OFX_UI_FONT_SMALL);
    gui->addLabel("'[' - PREVIOUS GUI", OFX_UI_FONT_SMALL);
    gui->addLabel("']' - NEXT GUI", OFX_UI_FONT_SMALL);

	//ofAddListener(gui->newGUIEvent, this, &GUI::variousGUIEvent);
    
    finaliseCanvas(gui);
}

void GUI::addDepthBoundsGUI()
{
    ofxUICanvas* gui = getNewGUI("DEPTH CLIPPING");
	
	gui->addToggle("TOGGLE CLIPPING", &app->sceneManager.isClipping, toggleSize, toggleSize);
	vector<string> modes;
	modes.push_back("LIVE");
	modes.push_back("RECORDED");
    gui->addRadio("CROPPING TYPE", modes, OFX_UI_ORIENTATION_VERTICAL, toggleSize, toggleSize);

    gui->addLabel("LIVE");
    gui->addSlider("LIVE NEAR CLIP", 0, 1, &app->sceneManager.liveNearClip, length, dim);
    gui->addSlider("LIVE FAR CLIP", 0, 1, &app->sceneManager.liveFarClip, length, dim);
    gui->addSlider("LIVE FLOOR NEAR CLIP", -0.5, 1.5, &app->sceneManager.liveNearFloorClip, length, dim);
    gui->addSlider("LIVE FLOOR FAR CLIP", -0.5, 1.5, &app->sceneManager.liveFarFloorClip, length, dim);
    gui->addSlider("LIVE CEILING NEAR CLIP", -0.5, 1.5, &app->sceneManager.liveNearCeilingClip, length, dim);
    gui->addSlider("LIVE CEILING FAR CLIP", -0.5, 1.5, &app->sceneManager.liveFarCeilingClip, length, dim);
    gui->addSlider("LIVE LEFT NEAR CLIP", -0.5, 1.5, &app->sceneManager.liveNearLeftClip, length, dim);
    gui->addSlider("LIVE LEFT FAR CLIP", -0.5, 1.5, &app->sceneManager.liveFarLeftClip, length, dim);
    gui->addSlider("LIVE RIGHT NEAR CLIP", -0.5, 1.5, &app->sceneManager.liveNearRightClip, length, dim);
    gui->addSlider("LIVE RIGHT FAR CLIP", -0.5, 1.5, &app->sceneManager.liveFarRightClip, length, dim);

	gui->addSpacer(length, 1);
    gui->addLabel("RECORDED");

    gui->addSlider("REC NEAR CLIP", 0, 1, &app->sceneManager.recNearClip, length, dim);
    gui->addSlider("REC FAR CLIP", 0, 1, &app->sceneManager.recFarClip, length, dim);
    gui->addSlider("REC FLOOR NEAR CLIP", -0.5, 1.5, &app->sceneManager.recNearFloorClip, length, dim);
    gui->addSlider("REC FLOOR FAR CLIP", -0.5, 1.5, &app->sceneManager.recFarFloorClip, length, dim);
    gui->addSlider("REC CEILING NEAR CLIP", -0.5, 1.5, &app->sceneManager.recNearCeilingClip, length, dim);
    gui->addSlider("REC CEILING FAR CLIP", -0.5, 1.5, &app->sceneManager.recFarCeilingClip, length, dim);
    gui->addSlider("REC LEFT NEAR CLIP", -0.5, 1.5, &app->sceneManager.recNearLeftClip, length, dim);
    gui->addSlider("REC LEFT FAR CLIP", -0.5, 1.5, &app->sceneManager.recFarLeftClip, length, dim);
    gui->addSlider("REC RIGHT NEAR CLIP", -0.5, 1.5, &app->sceneManager.recNearRightClip, length, dim);
    gui->addSlider("REC RIGHT FAR CLIP", -0.5, 1.5, &app->sceneManager.recFarRightClip, length, dim);
	
	ofAddListener(gui->newGUIEvent, this, &GUI::debugGUIEvent);
    finaliseCanvas(gui);
}


void GUI::addTrackingGUI()
{
    ofxUICanvas* gui = getNewGUI("TRACKING");
	
	gui->addSlider("CONFIDENCE MAX THRESHOLD", 0.5, 0.99, &app->poseManager.confidenceMaxThreshold, length, dim);
	gui->addSlider("CONFIDENCE EXPO", 1.0, 5.0, &app->poseManager.confidenceExpo, length, dim);
    gui->addSlider("MAX FRAMES FOR CONF TRIGGER", 5, 60, &app->sceneManager.recNearCeilingClip, length, dim);
	
    finaliseCanvas(gui);
}


void GUI::addSilhouetteGUI()
{
    ofxUICanvas* gui = getNewGUI("SILHOUETTE");
	
    gui->addLabel("VIDEO");
	gui->addSlider("VIDEO SILHOUETTE THRESHOLD", 0, 255, &app->sceneManager.videoSilhouetteThreshold, length, dim);
	
    finaliseCanvas(gui);
}


void GUI::addRecordingGUI()
{
    ofxUICanvas* gui = getNewGUI("RECORD-PLAYBACK");
    gui->addLabel("RECORDING");
	gui->addToggle("SOUND EVENT ACTIVE", &app->isSoundEventActive, toggleSize, toggleSize);
	gui->addSlider("MIN AMPLITUDE FOR EVENT", 0.1, 0.9, &app->minAmplitudeForEvent, length, dim);

    finaliseCanvas(gui);
}


void GUI::addDebugGUI()
{
    ofxUICanvas* gui = getNewGUI("DEBUG");
    gui->addLabel("PREVIEW");
	
	gui->addToggle("TOGGLE ALL DEBUG", &app->isDebugVisible, toggleSize, toggleSize);
	
	gui->addSpacer(length, 1);
    gui->addLabel("DEPTH");
	gui->addToggle("DRAW BODY INDEX", &app->kinectManager.isDrawBodyIndex, toggleSize, toggleSize);
	gui->addToggle("DRAW SKELETONS", &app->kinectManager.isDrawSkeletons, toggleSize, toggleSize);
	gui->addToggle("DRAW DEPTH", &app->sceneManager.isDrawDepth, toggleSize, toggleSize);
	gui->addToggle("DRAW CLIPPED DEPTH", &app->sceneManager.isDrawClippedDepth, toggleSize, toggleSize);
	
	gui->addSpacer(length, 1);
    gui->addLabel("SKELETAL");
	gui->addToggle("DRAW LIVE SKEL DATA", &app->poseManager.isLiveSkelelDataVisible, toggleSize, toggleSize);
	gui->addToggle("DRAW LOADED SKEL DATA", &app->poseManager.isLoadedSkelelDataVisible, toggleSize, toggleSize);
	gui->addSlider("SKEL PREVIEW SCALE", 0.1, 1.0, &app->poseManager.loadedPreviewScale, length, dim);

	finaliseCanvas(gui);
}

//void GUI::addDebugGUI()
//{
//}
//
//void GUI::addBackgroundGUI()
//{
//}





void GUI::debugGUIEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();
    
    if (name == "LIVE")
    {
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        if (toggle->getValue())
			app->sceneManager.isLiveClipping = true;
    }
    else if (name == "RECORDED")
    {
		ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        if (toggle->getValue())
			app->sceneManager.isLiveClipping = false;
    }
}