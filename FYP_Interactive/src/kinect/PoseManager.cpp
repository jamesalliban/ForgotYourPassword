#include "PoseManager.h"

void PoseManager::setup()
{
	fboW = 200;
	fboH = 350;

	recordedPose.type = "recorded";
	trackedPose.type = "tracked";
	livePose.type = "live";

	recordedPose.fbo.allocate(fboW, fboH);
	trackedPose.fbo.allocate(fboW, fboH);
	livePose.fbo.allocate(fboW, fboH);
	
	recordedPose.framesAtConfidenceLevel = 0;
	trackedPose.framesAtConfidenceLevel = 0;
	livePose.framesAtConfidenceLevel = 0;
	
	hasRecordingBeenTaken = false;

	loadSkelData();
}

void PoseManager::loadSkelData()
{
	ofDirectory dir;
	dir.listDir("xml");
	loadedPoses.clear();
	for (int i = 0; i < dir.size(); i++)
	{
		ofxXmlSettings skelData;
		if(skelData.loadFile(ofToDataPath("xml/" + dir.getFile(i).getFileName())))
		{
			Pose pose;
			pose.id = i;
			pose.type = "loaded";
			pose.fbo.allocate(fboW, fboH);
			
			skelData.pushTag("joints");
			int numberOfSavedPoints = skelData.getNumTags("joint");
			for(int j = 0; j < numberOfSavedPoints; j++)
			{
				skelData.pushTag("joint", j);
				ofVec3f pos;
				pos.x = skelData.getValue("x", 0.0);
				pos.y = skelData.getValue("y", 0.0);
				pos.z = skelData.getValue("z", 0.0);
				pose.joints.push_back(pos);
				skelData.popTag();
			}
			calculateCorrectedJoints(pose);
			calculateJointVectors(pose);
			drawPoseToFbo(pose);
			
			loadedPoses.push_back(pose);
		}
		else{
			ofLogError("Position file did not load!");
		}
		if (loadedPoses.size() > 0) hasRecordingBeenTaken = true;
	}
}


void PoseManager::update(vector<Body> bodies)
{
	if (isPoseAnalysisActive)
	{

	}

	if (hasRecordingBeenTaken)
		comparePoses(livePose, recordedPose);

	for (int i = 0; i < loadedPoses.size(); i++)
	{
		comparePoses(livePose, loadedPoses[i]);
	}


	if (bodies.size() > 0)
	{
		populatePose(bodies, livePose);
		calculateCorrectedJoints(livePose);
		calculateJointVectors(livePose);
	}
}


void PoseManager::draw()
{
	if (isLiveSkelelDataVisible)
	{
		drawPoseToFbo(livePose);
		
		ofPushStyle();
		ofSetColor(255);

		ofPushMatrix();
		ofTranslate(ofGetWidth() -fboW, 0);
		ofScale(1.0, 1.0);
		livePose.fbo.draw(0, 0);
		ofDrawBitmapString("Live", 10, 20);
		ofPopMatrix();
	
		ofPushMatrix();


		if (hasRecordingBeenTaken)
		{
			ofTranslate(ofGetWidth() - fboW, fboH);
			ofScale(1.0, 1.0); 
			trackedPose.fbo.draw(0, 0);
			ofDrawBitmapString("Recently tracked", 10, 20);
			ofPopMatrix();

			ofPushMatrix();
			ofTranslate(ofGetWidth() - fboW, fboH * 2);
			recordedPose.fbo.draw(0, 0);
			ofSetColor(255, 40);
			ofRect(0,  (-fboH * recordedPose.confidence) + fboH, fboW, fboH * recordedPose.confidence);
			ofSetColor(255);
			ofDrawBitmapString("Recorded Pose\n" + ofToString(recordedPose.confidence), 10, 20);
			ofPopMatrix();
		}
	}
	
	if (isLoadedSkelelDataVisible)
	{
		int maxRowSize = 900;
		int n = loadedPoses.size();
		for (int i = 0; i < n; i++)
		{
			float totalW = n * fboW * loadedPreviewScale;
			float totalRows = ceil(totalW / maxRowSize);
			int fbosPerRow = (float)maxRowSize / (float)(fboW * loadedPreviewScale);
			int thisRow = i / fbosPerRow;

			int x = fboW * (i % fbosPerRow) * loadedPreviewScale;
			int y = thisRow * fboH * loadedPreviewScale;

			Pose* pose = &loadedPoses[i];
			ofPushMatrix();
			ofTranslate(x, y);
			if (totalRows == 1)
				ofTranslate(ofGetWidth() - fboW - (n * fboW * loadedPreviewScale), 0);
			else
				ofTranslate(ofGetWidth() - fboW - (fbosPerRow * fboW * loadedPreviewScale), 0);
			ofScale(loadedPreviewScale, loadedPreviewScale);
			pose->fbo.draw(0, 0);
			ofSetColor(255, 40);
			ofRect(0,  (-fboH * pose->confidence) + fboH, fboW, fboH * pose->confidence);
			ofSetColor(255);
			ofDrawBitmapString("Pose " + ofToString(i) + "\n" + ofToString(pose->confidence), 10, 20);
			ofPopMatrix();
		}
	}

	ofPopStyle();
}



void PoseManager::recordNewPose(vector<Body> bodies)
{
	if (bodies.size() > 0)
	{
		hasRecordingBeenTaken = true;
		populatePose(bodies, recordedPose);
		calculateCorrectedJoints(recordedPose);
		calculateJointVectors(recordedPose);
		drawPoseToFbo(recordedPose);
		savePose();
		loadSkelData();
	}
}



//TODO: only track one person - the one standing in the zone
void PoseManager::populatePose(vector<Body> bodies, Pose & pose)
{
	int skelCount = bodies.size();
	for (int i = 0; i < skelCount; i++)
	{
		Body body = bodies[i];

		if (body.tracked)
		{
			pose.joints.clear();
			pose.joints.resize(25);

			pose.joints[JointType_SpineBase] = body.joints[JointType_SpineBase].getPosition();
			pose.joints[JointType_SpineMid] = body.joints[JointType_SpineMid].getPosition();
			pose.joints[JointType_Neck] = body.joints[JointType_Neck].getPosition();
			pose.joints[JointType_Head] = body.joints[JointType_Head].getPosition();
			pose.joints[JointType_ShoulderLeft] = body.joints[JointType_ShoulderLeft].getPosition();
			pose.joints[JointType_ElbowLeft] = body.joints[JointType_ElbowLeft].getPosition();
			pose.joints[JointType_WristLeft] = body.joints[JointType_WristLeft].getPosition();
			pose.joints[JointType_HandLeft] = body.joints[JointType_HandLeft].getPosition();
			pose.joints[JointType_ShoulderRight] = body.joints[JointType_ShoulderRight].getPosition();
			pose.joints[JointType_ElbowRight] = body.joints[JointType_ElbowRight].getPosition();
			pose.joints[JointType_WristRight] = body.joints[JointType_WristRight].getPosition();
			pose.joints[JointType_HandRight] = body.joints[JointType_HandRight].getPosition();
			pose.joints[JointType_HipLeft] = body.joints[JointType_HipLeft].getPosition();
			pose.joints[JointType_KneeLeft] = body.joints[JointType_KneeLeft].getPosition();
			pose.joints[JointType_AnkleLeft] = body.joints[JointType_AnkleLeft].getPosition();
			pose.joints[JointType_FootLeft] = body.joints[JointType_FootLeft].getPosition();
			pose.joints[JointType_HipRight] = body.joints[JointType_HipRight].getPosition();
			pose.joints[JointType_KneeRight] = body.joints[JointType_KneeRight].getPosition();
			pose.joints[JointType_AnkleRight] = body.joints[JointType_AnkleRight].getPosition();
			pose.joints[JointType_FootRight] = body.joints[JointType_FootRight].getPosition();
			pose.joints[JointType_SpineShoulder] = body.joints[JointType_SpineShoulder].getPosition();
			pose.joints[JointType_HandTipLeft] = body.joints[JointType_HandTipLeft].getPosition();
			pose.joints[JointType_ThumbLeft] = body.joints[JointType_ThumbLeft].getPosition();
			pose.joints[JointType_HandTipRight] = body.joints[JointType_HandTipRight].getPosition();
			pose.joints[JointType_ThumbRight] = body.joints[JointType_ThumbRight].getPosition();
			
		}
	}
}


void PoseManager::calculateCorrectedJoints(Pose & pose)
{
	// add corrected joint values - this is to draw the skeletons to the fbos
	pose.jointsCorrected.clear();
	pose.jointsCorrected.resize(25);
	for (int i = 0; i < pose.joints.size(); i++)
	{
		ofVec3f jointPos = pose.joints[i];
		jointPos.y *= -1;
		jointPos.z *= -1;
		jointPos.z += 2;
		jointPos *= 150;
		pose.jointsCorrected[i] = jointPos;
	}
}


void PoseManager::calculateJointVectors(Pose & pose)
{
	// add normalised joint vectors
	pose.jointVectors.clear();
	pose.jointVectors.resize(6);

	//pose.jointVectors[JointVectorType_Head] = ofVec3f(pose.joints[JointType_Head] - pose.joints[JointType_Neck]).getNormalized();

	pose.jointVectors[JointVectorType_ShoulderLeft] = ofVec3f(pose.joints[JointType_ShoulderLeft] - pose.joints[JointType_SpineShoulder]).getNormalized();
	pose.jointVectors[JointVectorType_UpperArmLeft] = ofVec3f(pose.joints[JointType_ElbowLeft] - pose.joints[JointType_ShoulderLeft]).getNormalized();
	pose.jointVectors[JointVectorType_ForearmLeft] = ofVec3f(pose.joints[JointType_WristLeft] - pose.joints[JointType_ElbowLeft]).getNormalized();
	//pose.jointVectors[JointVectorType_HandLeft] = ofVec3f(pose.joints[JointType_HandLeft] - pose.joints[JointType_WristLeft]).getNormalized();

	pose.jointVectors[JointVectorType_ShoulderRight] = ofVec3f(pose.joints[JointType_ShoulderRight] - pose.joints[JointType_SpineShoulder]).getNormalized();
	pose.jointVectors[JointVectorType_UpperArmRight] = ofVec3f(pose.joints[JointType_ElbowRight] - pose.joints[JointType_ShoulderRight]).getNormalized();
	pose.jointVectors[JointVectorType_ForearmRight] = ofVec3f(pose.joints[JointType_WristRight] - pose.joints[JointType_ElbowRight]).getNormalized();
	//pose.jointVectors[JointVectorType_HandRight] = ofVec3f(pose.joints[JointType_HandRight] - pose.joints[JointType_WristRight]).getNormalized();
}


void PoseManager::drawPoseToFbo(Pose & pose)
{		
	ofPushStyle();
	pose.fbo.begin();
	ofClear(1.0, 1.0, 1.0, 1.0);
	ofSetColor(0);
	ofRect(0, 0, fboW, fboH);
	
	ofPushMatrix();
	ofTranslate(fboW * 0.5, fboH * 0.5);
	if (pose.jointsCorrected.size() > 0)
	{
		ofSetColor(200, 255);
		ofSetLineWidth(2);
		ofLine(pose.jointsCorrected[JointType_Head], pose.jointsCorrected[JointType_Neck]);
		ofLine(pose.jointsCorrected[JointType_Neck], pose.jointsCorrected[JointType_SpineShoulder]);
		ofLine(pose.jointsCorrected[JointType_SpineShoulder], pose.jointsCorrected[JointType_SpineMid]);
		ofLine(pose.jointsCorrected[JointType_SpineMid], pose.jointsCorrected[JointType_SpineBase]);
		ofLine(pose.jointsCorrected[JointType_SpineShoulder], pose.jointsCorrected[JointType_ShoulderLeft]);
		ofLine(pose.jointsCorrected[JointType_ShoulderLeft], pose.jointsCorrected[JointType_ElbowLeft]);
		ofLine(pose.jointsCorrected[JointType_ElbowLeft], pose.jointsCorrected[JointType_WristLeft]);
		ofLine(pose.jointsCorrected[JointType_WristLeft], pose.jointsCorrected[JointType_HandLeft]);
		ofLine(pose.jointsCorrected[JointType_HandLeft], pose.jointsCorrected[JointType_HandTipLeft]);
		ofLine(pose.jointsCorrected[JointType_HandLeft], pose.jointsCorrected[JointType_ThumbLeft]);
		ofLine(pose.jointsCorrected[JointType_SpineShoulder], pose.jointsCorrected[JointType_ShoulderRight]);
		ofLine(pose.jointsCorrected[JointType_ShoulderRight], pose.jointsCorrected[JointType_ElbowRight]);
		ofLine(pose.jointsCorrected[JointType_ElbowRight], pose.jointsCorrected[JointType_WristRight]);
		ofLine(pose.jointsCorrected[JointType_WristRight], pose.jointsCorrected[JointType_HandRight]);
		ofLine(pose.jointsCorrected[JointType_HandRight], pose.jointsCorrected[JointType_HandTipRight]);
		ofLine(pose.jointsCorrected[JointType_HandRight], pose.jointsCorrected[JointType_ThumbRight]);
		ofLine(pose.jointsCorrected[JointType_SpineBase], pose.jointsCorrected[JointType_HipLeft]);
		ofLine(pose.jointsCorrected[JointType_HipLeft], pose.jointsCorrected[JointType_KneeLeft]);
		ofLine(pose.jointsCorrected[JointType_KneeLeft], pose.jointsCorrected[JointType_AnkleLeft]);
		ofLine(pose.jointsCorrected[JointType_AnkleLeft], pose.jointsCorrected[JointType_FootLeft]);
		ofLine(pose.jointsCorrected[JointType_SpineBase], pose.jointsCorrected[JointType_HipRight]);
		ofLine(pose.jointsCorrected[JointType_HipRight], pose.jointsCorrected[JointType_KneeRight]);
		ofLine(pose.jointsCorrected[JointType_KneeRight], pose.jointsCorrected[JointType_AnkleRight]);
		ofLine(pose.jointsCorrected[JointType_AnkleRight], pose.jointsCorrected[JointType_FootRight]);
	}
	ofPopMatrix();

	for (int i = 0; i < pose.jointsCorrected.size(); i++)
	{
		ofSetColor(255, 0, 0);
		ofPushMatrix();
		ofTranslate(fboW * 0.5, fboH * 0.5);
		ofVec3f pos = pose.jointsCorrected[i];
		ofTranslate(pos);
		ofCircle(0, 0, 5);
		ofPopMatrix();
	}
	
	ofSetColor(255);
	pose.fbo.end();
}


void PoseManager::comparePoses(Pose & live, Pose & recorded)
{
	if (live.jointVectors.size() == recorded.jointVectors.size())
	{
		float confidenceTotal = 0;
		int count = 0;
		for (int i = 0; i < live.jointVectors.size(); i++)
		{
			float mappedConfidence = ofMap(live.jointVectors[i].dot(recorded.jointVectors[i]), 0, 1, 0, 1, true);
			float expoConfidence = pow(mappedConfidence, confidenceExpo);
			confidenceTotal += expoConfidence;
			++count;
		}
		confidenceTotal /= count;
		recorded.confidence = int(confidenceTotal * 1000) / 1000.0;
		if (recorded.confidence > confidenceMaxThreshold)
		{
			recorded.framesAtConfidenceLevel++;
			if (recorded.framesAtConfidenceLevel >= maxFramesForConfTrigger)
			{
				cout << "EVENT - " << recorded.id << endl;
				recorded.framesAtConfidenceLevel = 0;
				trackedPose.joints = live.joints;
				trackedPose.jointsCorrected = live.jointsCorrected;
				drawPoseToFbo(trackedPose);
				ofNotifyEvent(poseRecognisedEvent, recorded);
			}
		}
		else
		{
			recorded.framesAtConfidenceLevel = 0;
		}
	}
}
			

void PoseManager::savePose()
{
	if (!hasRecordingBeenTaken)
		return;

	ofxXmlSettings skelData;
	skelData.addTag("joints");
	skelData.pushTag("joints");
	for (int i = 0; i < recordedPose.joints.size(); i++)
	{
		skelData.addTag("joint");
		skelData.pushTag("joint", i);
		skelData.addValue("x", recordedPose.joints[i].x);
		skelData.addValue("y", recordedPose.joints[i].y);
		skelData.addValue("z", recordedPose.joints[i].z);
		skelData.popTag();
	}
	skelData.popTag();
	
	skelData.saveFile(ofToDataPath("xml/" + getFileName() + ".xml"));
}


vector<ofImage> PoseManager::getPoseImages()
{
	vector<ofImage> poseImages;
	for (int i = 0; i < loadedPoses.size(); i++)
	{
		Pose &pose = loadedPoses[i];
		ofImage img;
		ofPixels pix;
		pose.fbo.readToPixels(pix);
		img.setFromPixels(pix);
		poseImages.push_back(img);
	}
	return poseImages;
}


string PoseManager::getFileName()
{
	string year = ofToString(ofGetYear());
    string month = ofToString(ofGetMonth());
    if (month.size() == 1) month.insert(0, "0");
    string day = ofToString(ofGetDay());
    if (day.size() == 1) day.insert(0, "0");
    string hour = ofToString(ofGetHours());
    if (hour.size() == 1) hour.insert(0, "0");
    string minute = ofToString(ofGetMinutes());
    if (minute.size() == 1) minute.insert(0, "0");
    string second = ofToString(ofGetSeconds());
    if (second.size() == 1) second.insert(0, "0");
    
    return "recording_" + year + "-" + month + "-" + day + "_" + hour + "." + minute + "." + second;
}