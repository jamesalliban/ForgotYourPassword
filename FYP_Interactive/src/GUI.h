#pragma once

#include "ofMain.h"
#include "ofxUIManager.h"

class ofApp;

class GUI : public ofxUIManager
{
public:
    virtual void setup();

    void addVariousGUI();
	void addDepthBoundsGUI();
	void addTrackingGUI();
	void addSilhouetteGUI();
	void addRecordingGUI();
    void addDebugGUI();
    //void addBackgroundGUI();
    
    void debugGUIEvent(ofxUIEventArgs &e);
    
    ofApp* app;
};