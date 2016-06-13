//
//  main.cpp
//  HandTracking
//
//  Created by Daniel Gallagher on 13/06/2016.
//  Copyright (c) 2016 Developer Co. All rights reserved.
//

#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;

Mat frame, edges;

Mat foregroundMask;

Ptr<BackgroundSubtractor> pMOG2;
int keyboard;

int main(int argc, const char * argv[]) {
    VideoCapture cap(0);
    
    if(!cap.isOpened())
        return -1;
    
    pMOG2 = createBackgroundSubtractorMOG2();
    
    while(true)
    {
        
        cap >> frame;
    
        cvtColor(frame, frame, CV_BGR2GRAY);
        
        pMOG2->apply(frame, foregroundMask);
        
        stringstream ss;
        
        rectangle(frame, Point(10, 2), Point(100,20), Scalar(255,255,255), -1);
        
        ss << cap.get(CAP_PROP_POS_FRAMES);
        
        string frameNumberString = ss.str();
        
        putText(frame, frameNumberString.c_str(), Point(15, 15),
                FONT_HERSHEY_COMPLEX, 0.5, Scalar(0,0,0));
        
        imshow("Frame", frame);
        imshow("Mask", foregroundMask);
        
        if(waitKey(30) >= 0)
            break;
    }
    
    return 0;
}




