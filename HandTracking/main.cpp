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

Mat frame, filtered;

Mat foregroundMask;

Ptr<BackgroundSubtractor> bgSub;

int main(int argc, const char * argv[]) {
    
    VideoCapture cap(0);
    
    //bgSub = createBackgroundSubtractorMOG2();
    
    if(!cap.isOpened())
        return -1;
    
    while(true)
    {
        
        cap >> frame;
    
        cvtColor(frame, filtered, CV_BGR2GRAY);
        GaussianBlur(filtered, filtered, Size(7,7), 1.5);
        
        //Canny(edges, edges, 0, 30);
        
        //bgSub->apply(frame, foregroundMask, -1);
        
        Mat frame_gray;
        Mat threshold_output;
        
        vector< vector<Point> > contours;
        vector <Vec4i> hierarchy;
        
        
        //Find contours
        cvtColor(frame, frame_gray, CV_BGR2GRAY);
        threshold(filtered, threshold_output, 200, 255, THRESH_BINARY);
        
        imshow("Hand", threshold_output);
        
        findContours(threshold_output, contours, hierarchy,
                     CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
        
        //find convex hull for each object
        vector < vector<Point> > hull(contours.size());
        
        for (int i=0; i < contours.size(); i++)
        {
            convexHull(Mat(contours[i]), hull[i], false);
        }
        
        //draw contours and hull results
        RNG rand;
        Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
        
        for (int i = 0; i < contours.size(); i++)
        {
            Scalar colour = Scalar(rand.uniform(0, 255),
                                   rand.uniform(0, 255),
                                   rand.uniform(0, 255));
            
            drawContours(drawing, contours, i, colour, 1, 8, vector<Vec4i>(), 0, Point());
            
            drawContours(drawing, hull, i, colour, 1, 8, vector<Vec4i>(), 0, Point());
        }
        
        
        namedWindow("Hand Recognition");
        imshow("Hand Recognition", drawing);
        
        if(waitKey(30) >= 0)
            break;
    }
    
    return 0;
}





















