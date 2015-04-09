#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "ofxXmlSettings.h"

using namespace ofxKinectForWindows2::Data;

struct Pose
{
	string type;
	int id;
	vector<ofVec3f> joints;
	vector<ofVec3f> jointsCorrected;
	vector<ofVec3f> jointVectors;
	ofFbo fbo;
	float confidence;
	int framesAtConfidenceLevel;
};

enum _JointVectorType
    {
        //JointVectorType_Head = 0, // neck - centre spine 
        JointVectorType_ShoulderLeft = 0, // left shoulder - centre spine
        JointVectorType_UpperArmLeft = 1, // left elbow - left shoulder 
        JointVectorType_ForearmLeft = 2, // left wrist - left elbow
        JointVectorType_HandLeft = 3, // left hand - left wrist
        JointVectorType_ShoulderRight = 4,
        JointVectorType_UpperArmRight = 5,
        JointVectorType_ForearmRight = 6,
        JointVectorType_HandRight = 7,
    } ;

class PoseManager
{
public:
	void setup();
	void loadSkelData();
	void update(vector<Body> bodies);
	void draw();
	void recordNewPose(vector<Body> bodies);
	void populatePose(vector<Body> bodies, Pose & pose);
	void calculateCorrectedJoints(Pose & pose);
	void calculateJointVectors(Pose & pose);
	void drawPoseToFbo(Pose & pose);
	void comparePoses(Pose & live, Pose & recorded);
	void savePose();
	string getFileName();
	
	Pose recordedPose;
	Pose trackedPose;
	Pose livePose;
	vector<Pose> loadedPoses;
	bool isPoseAnalysisActive;
	int fboW, fboH;

	float loadedPreviewScale;
	bool isLiveSkelelDataVisible;
	bool isLoadedSkelelDataVisible;
	
	float confidenceMaxThreshold;
	float framesAtConfidenceLevel;
	float confidenceExpo;

	ofEvent<Pose> poseRecognisedEvent;

	bool hasRecordingBeenTaken;
};