#pragma once

#include "ofMain.h"
#include "ofxUIManager.h"

class ofApp;

class GUI : public ofxUIManager
{
public:
    virtual void setup(int sequenceSize, vector<ofImage> skelImages);

    void addVariousGUI();
	void addDepthBoundsGUI();
	void addTrackingGUI();
	void addUniversalSilhouetteGUI();
	void addDancerSilhouetteGUI();
	void addUserSilhouetteGUI();
	void addSilhouetteTransitionGUI();
	void addSequenceSelectorGUI(int sequenceSize, vector<ofImage> skelImages);
	void addSoundGUI();
	void addInstructionsGUI();
	void addRecordingGUI();
    void addDebugGUI();
    void addBackgroundGUI();
    
    void debugGUIEvent(ofxUIEventArgs &e);
    void sequenceSelectorGUIEvent(ofxUIEventArgs &e);
    
    ofApp* app;
};