#include "GUI.h"
#include "ofApp.h"

void GUI::setup(int sequenceSize, vector<ofImage> skelImages)
{
    app = (ofApp*)ofGetAppPtr();
    ofxUIManager::setup(350);
	
    addVariousGUI();
    addDepthBoundsGUI();
    addTrackingGUI();
	addUniversalSilhouetteGUI();
	addDancerSilhouetteGUI();
	addUserSilhouetteGUI();
	addSequenceSelectorGUI(sequenceSize, skelImages);
    addSoundGUI();
	addInstructionsGUI();
	addRecordingGUI();
    addDebugGUI();
    addBackgroundGUI();
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
    gui->addLabel("'p' - TOGGLE PAUSE", OFX_UI_FONT_SMALL);
    gui->addLabel("'f' - TOGGLE FULLSCREEN", OFX_UI_FONT_SMALL);
    gui->addLabel("'t' - TOGGLE TRACKING", OFX_UI_FONT_SMALL);
    gui->addLabel("'l' - LOAD SHADERS", OFX_UI_FONT_SMALL);
    gui->addLabel("'d' - TOGGLE DEBUG VISIBLE", OFX_UI_FONT_SMALL);

    gui->addLabel("'r' - TOGGLE RECORDING", OFX_UI_FONT_SMALL);
    gui->addLabel("'S' - SAVE NEW POSE TO XML", OFX_UI_FONT_SMALL);
	
    gui->addLabel("'0-9' - PLAY VIDEO", OFX_UI_FONT_SMALL);
    gui->addLabel("'s' - STOP CURRENT VIDEO", OFX_UI_FONT_SMALL);
    gui->addLabel("'LEFT' - SCRUB VIDEO BACK 1 SECOND", OFX_UI_FONT_SMALL);
    gui->addLabel("'RIGHT' - SCRUB VIDEO FORWARD 1 SECOND", OFX_UI_FONT_SMALL);

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
	
	gui->addToggle("TOGGLE TRACKING", &app->poseManager.isTracking, toggleSize, toggleSize);
	gui->addSlider("CONFIDENCE MAX THRESHOLD", 0.5, 0.99, &app->poseManager.confidenceMaxThreshold, length, dim);
	gui->addSlider("CONFIDENCE EXPO", 1.0, 5.0, &app->poseManager.confidenceExpo, length, dim);
    gui->addSlider("MAX FRAMES FOR CONF TRIGGER", 5, 60, &app->poseManager.maxFramesForConfTrigger, length, dim);
	
    finaliseCanvas(gui);
}


void GUI::addUniversalSilhouetteGUI()
{
    ofxUICanvas* gui = getNewGUI("UNIVERSAL SILHOUETTE");
	
	gui->addLabel("COLOUR");
	gui->addSlider("RED", 0, 255, &app->sceneManager.silhouetteCol[0], length, dim);
	gui->addSlider("GREEN", 0, 255, &app->sceneManager.silhouetteCol[1], length, dim);
	gui->addSlider("BLUE", 0, 255, &app->sceneManager.silhouetteCol[2], length, dim);


    finaliseCanvas(gui);
}


void GUI::addDancerSilhouetteGUI()
{
    ofxUICanvas* gui = getNewGUI("DANCER SILHOUETTE");
	
    gui->addLabel("DANCER");
	gui->addSlider("IMAGE CONTOUR THRESHOLD", 0, 255, &app->sceneManager.dancerSilhouette.imageContourThreshold, length, dim);
	gui->addSlider("IMAGE BLUR AMOUNT", 0, 20, &app->sceneManager.dancerBlurAmount, length, dim);
	gui->addSlider("IMAGE BLUR SAMPLES", 0, 20, &app->sceneManager.dancerBlurSamples, length, dim);
	
	gui->addSpacer(length, 1);
	gui->addSlider("CV THRESHOLD", 1, 255, &app->sceneManager.dancerSilhouette.cvThreshold, length, dim);
	gui->addSlider("MIN CONTOUR AREA", 1, 200, &app->sceneManager.dancerSilhouette.minContourArea, length, dim);
	gui->addSlider("MAX CONTOUR AREA", 1, 100000, &app->sceneManager.dancerSilhouette.maxContourArea, length, dim);
	gui->addSlider("CONTOUR AMOUNT CONSIDERED", 1, 200, &app->sceneManager.dancerSilhouette.contourAmountConsidered, length, dim);
	gui->addToggle("CONTOUR FIND HOLES", &app->sceneManager.dancerSilhouette.isFindHoles, toggleSize, toggleSize);
	gui->addToggle("CONTOUR USE APPROXIMATION", &app->sceneManager.dancerSilhouette.isUseApproximation, toggleSize, toggleSize);

	gui->addSpacer(length, 1);
	gui->addSlider("CONTOUR RESAMPLE COUNT", 3, 200, &app->sceneManager.dancerSilhouette.resampleAmount, length, dim);
	
	gui->addSpacer(length, 1);
	gui->add2DPad("POSITION", ofPoint(0, 1), ofPoint(0, 1), &app->sceneManager.dancerSilhouette.position, length, length * 0.6);;
	gui->addSlider("SCALE", 0.1, 10, &app->sceneManager.dancerSilhouette.scale, length, dim);
	
    finaliseCanvas(gui);
}


void GUI::addUserSilhouetteGUI()
{
    ofxUICanvas* gui = getNewGUI("USER SILHOUETTE");
	
    gui->addLabel("USER");
	gui->addSlider("IMAGE CONTOUR THRESHOLD", 0, 255, &app->sceneManager.userSilhouette.imageContourThreshold, length, dim);
	gui->addSlider("IMAGE BLUR AMOUNT", 0, 20, &app->sceneManager.userBlurAmount, length, dim);
	gui->addSlider("IMAGE BLUR SAMPLES", 0, 20, &app->sceneManager.userBlurSamples, length, dim);
	
	gui->addSpacer(length, 1);
	gui->addSlider("CV THRESHOLD", 1, 255, &app->sceneManager.userSilhouette.cvThreshold, length, dim);
	gui->addSlider("MIN CONTOUR AREA", 1, 200, &app->sceneManager.userSilhouette.minContourArea, length, dim);
	gui->addSlider("MAX CONTOUR AREA", 1, 100000, &app->sceneManager.userSilhouette.maxContourArea, length, dim);
	gui->addSlider("CONTOUR AMOUNT CONSIDERED", 1, 200, &app->sceneManager.userSilhouette.contourAmountConsidered, length, dim);
	gui->addToggle("CONTOUR FIND HOLES", &app->sceneManager.userSilhouette.isFindHoles, toggleSize, toggleSize);
	gui->addToggle("CONTOUR USE APPROXIMATION", &app->sceneManager.userSilhouette.isUseApproximation, toggleSize, toggleSize);
	
	gui->addSpacer(length, 1);
	gui->addSlider("CONTOUR RESAMPLE COUNT", 3, 200, &app->sceneManager.userSilhouette.resampleAmount, length, dim);
	
	gui->addSpacer(length, 1);
	gui->add2DPad("POSITION", ofPoint(0, 1), ofPoint(0, 1), &app->sceneManager.userSilhouette.position, length, length * 0.6);;
	gui->addSlider("SCALE", 0.1, 10, &app->sceneManager.userSilhouette.scale, length, dim);

    finaliseCanvas(gui);
}


void GUI::addSequenceSelectorGUI(int sequenceSize, vector<ofImage> skelImages)
{
    ofxUICanvas* gui = getNewGUI("SEQUENCE");
	
	ofDirectory dir;
	dir.listDir("images");
	

	for (int i = 0; i < dir.size(); i++)
	{
		ofxUIImageButton * btn = new ofxUIImageButton(100, 80, false, "images/" + dir.getFile(i).getFileName(), "SQ " + ofToString(i));
		if (i % 3 == 0) 
			gui->addWidgetDown(btn);
		else
			gui->addWidgetRight(btn);
	}

    finaliseCanvas(gui);
	ofAddListener(gui->newGUIEvent, this, &GUI::sequenceSelectorGUIEvent);
}


void GUI::addSoundGUI()
{
    ofxUICanvas* gui = getNewGUI("SOUND");
	gui->addSlider("INTRO SPEED", 1, 300, &app->soundManager.introSpeed, length, dim);
	gui->addSlider("OUTRO SPEED", 1, 300, &app->soundManager.outroSpeed, length, dim);
	finaliseCanvas(gui);
}


void GUI::addInstructionsGUI()
{
    ofxUICanvas* gui = getNewGUI("INSTRUCTIONS");
	
	gui->addSpacer(length, 1);
	gui->addSlider("CV THRESHOLD", 1, 255, &app->sceneManager.instructions.cvThreshold, length, dim);
	gui->addSlider("MIN CONTOUR AREA", 1, 200, &app->sceneManager.instructions.minContourArea, length, dim);
	gui->addSlider("MAX CONTOUR AREA", 1, 100000, &app->sceneManager.instructions.maxContourArea, length, dim);
	gui->addSlider("CONTOUR AMOUNT CONSIDERED", 1, 200, &app->sceneManager.instructions.contourAmountConsidered, length, dim);
	gui->addToggle("CONTOUR FIND HOLES", &app->sceneManager.instructions.isFindHoles, toggleSize, toggleSize);
	gui->addToggle("CONTOUR USE APPROXIMATION", &app->sceneManager.instructions.isUseApproximation, toggleSize, toggleSize);
	
	gui->addSpacer(length, 1);
	gui->addSlider("CONTOUR RESAMPLE COUNT", 3, 200, &app->sceneManager.instructions.resampleAmount, length, dim);
	gui->addSlider("RANDOM OFFSET", 0, 5.0, &app->sceneManager.instructions.randOffset, length, dim);
	
	gui->addSpacer(length, 1);
	gui->addSlider("TEXT 1 Y POSITION", 0, 1200, &app->sceneManager.instructions.text1YPos, length, dim);
	gui->addSlider("TEXT 2 Y POSITION", 0, 1200, &app->sceneManager.instructions.text2YPos, length, dim);
	gui->addSlider("POSE SELECTION Y POSITION", 0, 1200, &app->sceneManager.instructions.poseSelectionYPos, length, dim);
	gui->addSlider("POSE SELECTION SPACING", 0, 300, &app->sceneManager.instructions.poseSelectionSpacing, length, dim);
	gui->addSlider("POSE SELECTION SCALE", 0, 2, &app->sceneManager.instructions.poseSelectionScale, length, dim);
	

	finaliseCanvas(gui);
}


void GUI::addRecordingGUI()
{
    ofxUICanvas* gui = getNewGUI("RECORD-PLAYBACK");
    gui->addLabel("RECORDING");
	gui->addToggle("SOUND EVENT ACTIVE", &app->isSoundEventActive, toggleSize, toggleSize);
	gui->addSlider("MIN AMPLITUDE FOR EVENT", 0.01, 0.2, &app->minAmplitudeForEvent, length, dim);
	gui->addSlider("FRAMES FOR SOUND EVENT", 2, 30, &app->framesForSoundEvent, length, dim);
    finaliseCanvas(gui);
}



void GUI::addBackgroundGUI()
{
    ofxUICanvas* gui = getNewGUI("BACKGROUND");
	
	gui->addSlider("BG RED", 0, 255, &app->sceneManager.bgCol[0], length, dim);
	gui->addSlider("BG GREEN", 0, 255, &app->sceneManager.bgCol[1], length, dim);
	gui->addSlider("BG BLUE", 0, 255, &app->sceneManager.bgCol[2], length, dim);
	
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
	gui->addToggle("DRAW LARGE CLIPPED DEPTH", &app->sceneManager.isDrawLargeClippedDepth, toggleSize, toggleSize);
	gui->addSlider("LARGE CLIPPED DEPTH SCALE", 1.0, 3.0, &app->sceneManager.largeClippingScale, length, dim);
	
	gui->addSpacer(length, 1);
    gui->addLabel("SILHOUETTE");
	gui->addToggle("DANCER SILHOUETTE VISIBLE", &app->sceneManager.isDancerSilhouetteDebugVisible, toggleSize, toggleSize);
	gui->addToggle("USER SILHOUETTE VISIBLE", &app->sceneManager.isUserSilhouetteDebugVisible, toggleSize, toggleSize);
	gui->addSlider("SILHOUETTE PREVIEW SCALE", 0.1, 1.0, &app->sceneManager.vidScale, length, dim);

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


void GUI::sequenceSelectorGUIEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName().substr(3);
	cout << "SQ - - - - - - '" << name << "'" << endl;
	app->sceneManager.playVideo(ofToInt(name));
}
