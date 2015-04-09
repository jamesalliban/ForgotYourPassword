//
//  ScreenRecorder.h
//  emptyExample
//
//  Created by James Alliban's MBP on 03/06/2014.
//
//

#pragma once

//#define IMAGE_FORMAT_TGA "tga"
//#define IMAGE_FORMAT_BMP "bmp"
//#define IMAGE_FORMAT_TIFF "tiff"
//#define IMAGE_FORMAT_JPG "jpg"
//#define IMAGE_FORMAT_PNG "png"

#include "ofMain.h"
#include "ThreadedImageSequenceExporter.h"

struct SecondData
{
	int currentSecond;
	int frameCount;
};

class ScreenRecorder
{
public:
    void setup();
	void draw();
    void startRecording();
    void stopRecording();
    void setupNewDirectory();
    void addFrame(ofPixels pix);
    void exit();
    
    void setDesiredFramerate(int fps) { desiredFramerate = fps; };
    int getDesiredFramerate() { return desiredFramerate; };
    bool getIsRecording() { return isRecording; };
    void setIsRecording(bool val) { isRecording = val; };
    
    string getImageFormat() { return imageFormat; };
    void setImageFormat(string val) { imageFormat = val; };
    
    string getLastRecordedPath() { return currentPath; };
    
    int getQueueSize() { return recorder.q.size(); };
    int getExportedSize();
    void deleteLastCreatedDir();
    
    
private:
    
    ThreadedImageSequenceExporter recorder;
    ofFile newFile;
    ofDirectory dir;
    
    int desiredFramerate;
    string rootFolderName;
    string currentPath;
    bool isRecording;
    long long millisAtLastFrame;
    
    int counter;
    string imageFormat;

	vector<SecondData> frameExportData;
    
};