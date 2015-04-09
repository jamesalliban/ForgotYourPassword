//
//  ScreenRecorder.cpp
//  emptyExample
//
//  Created by James Alliban's MBP on 03/06/2014.
//
//

#include "ScreenRecorder.h"


void ScreenRecorder::setup()
{
    rootFolderName = ofToDataPath("recordings");
    imageFormat = "tga";
    recorder.startThread(false, true);
    isRecording = false;
    
    currentPath = rootFolderName + "/blah"; // 'blah' is used so we don't accidentally delete anything we shouldn't
    millisAtLastFrame = 0;
}


void ScreenRecorder::draw()
{
	string frameDataStr;
	for (int i = 0; i < frameExportData.size(); i++)
	{
		frameDataStr += ofToString(i) + " - " + ofToString(frameExportData[i].frameCount) + "\n";
	}
	ofDrawBitmapString(frameDataStr, 0, 0);
}


void ScreenRecorder::startRecording()
{
    recorder.clear();
    frameExportData.clear();
	isRecording = true;
    setupNewDirectory();
}


void ScreenRecorder::stopRecording()
{
    isRecording = false;
}

void ScreenRecorder::setupNewDirectory()
{
    counter = 0;
    
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
    
    currentPath = rootFolderName + "/recording_" + year + "-" + month + "-" + day + "_" + hour + "." + minute + "." + second;
    
    printf("rootFolderName = %s, currentPath = %s \n", rootFolderName.c_str(), currentPath.c_str());
    ofDirectory dir(currentPath);
    if(!dir.exists())
        dir.create(true);
    
    millisAtLastFrame = ofGetElapsedTimeMillis();
}


void ScreenRecorder::addFrame(ofPixels pix)
{
    int millisSinceLastFrame = ofGetElapsedTimeMillis() - millisAtLastFrame;
    char fileName[100];
    //sprintf(fileName,  "/frame_%s_%s_%.8i.%s",  ofToString(ofGetElapsedTimef()).c_str(), ofToString(millisSinceLastFrame).c_str(), counter, imageFormat.c_str());
	sprintf(fileName,  "/frame_%.8i_%s_%s.%s", counter, ofToString(ofGetElapsedTimef()).c_str(), ofToString(millisSinceLastFrame).c_str(), imageFormat.c_str());
//    sprintf(fileName,  "/frame%.8i_%s_.%s",  counter, ofToString(millisSinceLastFrame).c_str(), imageFormat.c_str());
    //sprintf(fileName,  "/frame_%.8i.%s",  counter, imageFormat.c_str());
    
    counter++;
    
    string pathAndFileName = currentPath + fileName;
    
    recorder.addFrame(pix, pathAndFileName);
    
    millisAtLastFrame = ofGetElapsedTimeMillis();

	
	if (frameExportData.size() == 0)
	{
		SecondData secondData;
		secondData.currentSecond = (int)ofGetElapsedTimef();
		secondData.frameCount = 1;
		frameExportData.push_back(secondData);
	}

	if (frameExportData.back().currentSecond == (int)ofGetElapsedTimef())
	{
		frameExportData.back().frameCount++;
	}
	else
	{
		SecondData secondData;
		secondData.currentSecond = (int)ofGetElapsedTimef();
		secondData.frameCount = 1;
		frameExportData.push_back(secondData);
	}
}


int ScreenRecorder::getExportedSize()
{
    return recorder.counter;
}



void ScreenRecorder::exit()
{
    recorder.waitForThread();
}