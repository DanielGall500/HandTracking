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

int BskinMin = 130, GskinMin = 160, RskinMin = 230;
int BskinMax = 200, GskinMax = 220, RskinMax = 255;

bool inRange(int val, int min, int max);
Mat findSkin(Mat frame);

int main(int argc, const char * argv[]) {
    
    VideoCapture cap(0); //always BGR
    
    if(!cap.isOpened())
        return -1;
    
    namedWindow("Testing");
    
    createTrackbar("BlueMin", "Testing", &BskinMin, 255);
    createTrackbar("GreenMin", "Testing", &GskinMin, 255);
    createTrackbar("RedMin", "Testing", &RskinMin, 255);
    
    createTrackbar("BlueMax", "Testing", &BskinMax, 255);
    createTrackbar("GreenMax", "Testing", &GskinMax, 255);
    createTrackbar("RedMax", "Testing", &RskinMax, 255);
    
    while(true)
    {
        Mat frame, blurred;
        
        cap >> frame;
        
        GaussianBlur(frame, blurred, Size(7,7), 1.5);
        
        Mat newFrame = findSkin(blurred);
        
        cout << newFrame.at<Vec3b>(0,0) << endl;
        
        imshow("Testing", newFrame);
        
        if(waitKey(30) > 0)
            return -1;
        
    }
    
    return 0;
}

bool withinRange(int val, int min, int max) //efficiently checks whether val is in between min and max
{
    return (unsigned)(val - min) <= (max - min);
}

Mat findSkin(Mat frame)
{
    Vec3b colChannels;
    int b, g, r;
    
    for (int rows = 0; rows <= frame.rows; rows++)
    {
        for (int cols = 0; cols <= frame.cols; cols++)
        {
            colChannels = frame.at<Vec3b>(rows, cols);
            
            b = colChannels[0];
            g = colChannels[1];
            r = colChannels[2];
            
            /*if(withinRange(b, BskinMin, BskinMax) &&
               withinRange(g, GskinMin, GskinMax) &&
               withinRange(r, RskinMin, RskinMax))*/
            
            if(withinRange(b, BskinMin, BskinMax) &&
               withinRange(g, GskinMin, GskinMax) &&
               withinRange(r, RskinMin, RskinMax))
            {
                colChannels = Vec3b(255,255,255);
            }
            else
            {
                colChannels = Vec3b(0,0,0);
            }
            
            frame.at<Vec3b>(rows, cols) = colChannels;
            
        }
    }
    return frame;
}




















