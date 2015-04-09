/*
 
 A customised version of atduskgreg's ofxImageSequenceRecorder addon
 
 Which was based on code by Memo from this thread:
 http://forum.openframeworks.cc/index.php?topic=1687.0
 
 */


#include "ofMain.h"    

typedef struct { 
    string fileName;    
    ofPixels image;
} QueuedImage;

class ThreadedImageSequenceExporter : public ofThread
{
public:    
  
    int counter;  
    queue<QueuedImage> q;
    
    ThreadedImageSequenceExporter()
    {
        counter = 0;
    }
       
    void threadedFunction()
    {
		cout << "exporting" << endl;
        while(isThreadRunning())
        {
            if(!q.empty())
            {
                QueuedImage i = q.front();
                ofSaveImage(i.image, i.fileName);
                ++counter;
                q.pop();
            }
        }
    }   
    
    void addFrame(ofImage &img, string fileName)
    {
        addFrame(img.getPixelsRef(), fileName);
    }
    
    void addFrame(ofVideoGrabber &cam, string fileName)
    {
        addFrame(cam.getPixelsRef(), fileName);
    }
    
    void addFrame(ofVideoPlayer &player, string fileName)
    {
        addFrame(player.getPixelsRef(), fileName);
    }
        
    void addFrame(ofPixels imageToSave, string fileName)
    {
        
        QueuedImage qImage;
        
        qImage.fileName = fileName;
        qImage.image = imageToSave; 
        
        q.push(qImage);
    }
    
    void clear()
    {
        q.empty();
        counter = 0;
    }
};  