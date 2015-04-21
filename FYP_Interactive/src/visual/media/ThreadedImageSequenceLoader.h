//
//  ThreadedImageSequenceLoader.h
//  Mumm_Cork
//
//  Created by James Alliban's MBP on 05/06/2014.
//
//



#include "ofMain.h"

typedef struct {
    string fileName;
    ofPixels pix;
} QueuedSequenceImage;

class ThreadedImageSequenceLoader : public ofThread
{
public:
    
    queue<QueuedSequenceImage> q;
    string loadPath;
    int counter;
    int imagesPlayed;
    ofDirectory dir;
	int dirSize;
	ofImage tmpImg;
	ofPixels tmpPix;
    
    ThreadedImageSequenceLoader()
    {
    }
    
    void threadedFunction()
    {
        while(isThreadRunning())
        {
			//cout << ofGetFrameNum() << " counter = " << counter << ",  dir.getFiles().size() = " <<  dir.getFiles().size() << endl;
            if (counter < dir.getFiles().size())
            {
				if (q.size() < 20)
				{
					QueuedSequenceImage qImage;
					ofFile file = dir.getFile(counter);
					qImage.fileName = file.getFileName();
					//qImage.pix = ofPixels();
					//ofImage img;
					//img.setUseTexture(false);
					//img.loadImage(file);
					//cout << (qImage.pix, loadPath + "/" + file.getFileName()) << endl;
					ofLoadImage(qImage.pix, loadPath + "/s/" + file.getFileName());
					//qImage.image = img;
					q.push(qImage);
					counter++;
				}
            }
            else
            {
                stopThread();
            }
        }
    }
    
    void setLoadPath(string _loadPath)
    {
        loadPath = _loadPath;
        //loadPath = "image_sequences/recording_2015-04-02_14.23.43";
        dir.listDir(loadPath);
		dirSize = dir.getFiles().size();
	}

    void startLoading()
    {
        counter = 0;
        startThread(false, true);
    }

	ofPixels getImage()
	{
		//tmpImg = q.front().image;
		tmpPix = q.front().pix;
		q.pop();
		return tmpPix;
	}    

    void clear()
    {
        q.empty();
    }
};