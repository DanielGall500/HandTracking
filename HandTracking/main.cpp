//
//  main.cpp
//  HandTracking
//
//  Created by Daniel Gallagher on 13/06/2016.
//  Copyright (c) 2016 Developer Co. All rights reserved.
//

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include "BinarySkinFilter.h"
#include "HandElementContainer.h"

#include <iostream>
#include <string.h>
#include <sstream>
#include <fstream>
#include <time.h>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace cv;
using namespace std;

bool isHandExpanded(vector<int> fingerAngles, int threshDegree);

Rect extRectOne, extRectTwo, extRectThree, extRectFour,
extRectFive, extRectSix, extRectSeven, extRectEight, extRectNine, extRectTen;

vector<vector<Point>> handContours;
vector<vector<Point>> handHullPoints;
vector<vector<int>> handHullInts;
vector<vector<Vec4i>> handDefects;

vector<int> fingerAngles;

HandElementContainer ptManager;

int contrastMultiplier = 2;

void read_images(std::string imgs_filename, vector<Mat> &images, vector<int> labels, char seperator = ';')
{
    std::stringstream fileStream(imgs_filename.c_str(), ifstream::in);
    
    if(!fileStream)
        CV_Error(CV_StsBadArg, "Invalid Filestream Input");
    
    string line, path, classLabel;
    
    while (getline(fileStream, line))
    {
        stringstream lineSS(line);
        
        getline(lineSS, path, seperator);
        
        getline(lineSS, classLabel);
        
        if(!path.empty() && !classLabel.empty())
        {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classLabel.c_str()));
        }
    }
    
}

int main(int argc, const char * argv[]) {
    
    VideoCapture cap(0); //always BGR
    
    int frameCounter = -1;
    
    if(!cap.isOpened())
        return -1;
    
    BinarySkinFilter skinFilter(contrastMultiplier);
    int binaryThreshold = 25;
    
    HandElementContainer handElements;
    
    Mat firstFrame;
    cap >> firstFrame;
    
    int wCenter = firstFrame.size().width / 2;
    int hCenter = firstFrame.size().height / 2;
    
    extRectOne = Rect(Point(wCenter - 100, hCenter + 70), Point(wCenter + 100, hCenter + 50)); // -(50 30)
    extRectTwo = Rect(Point(wCenter - 100, hCenter + 150), Point(wCenter + 100, hCenter + 130)); //50 30
    extRectThree = Rect(Point(wCenter - 100, hCenter + 10), Point(wCenter + 100, hCenter - 10)); //50 30
    
    extRectFour = Rect(Point(wCenter - 120, hCenter - 240), Point(wCenter - 100, hCenter - 80));
    
    extRectFive = Rect(Point(wCenter - 70, hCenter - 280), Point(wCenter - 50, hCenter - 80));
    extRectSix = Rect(Point(wCenter + 70, hCenter - 280), Point(wCenter + 50, hCenter - 80));
    extRectSeven = Rect(Point(wCenter - 10, hCenter - 310), Point(wCenter + 10, hCenter - 110));
    
    extRectEight = Rect(Point(wCenter + 140, hCenter - 50), Point(wCenter + 120, hCenter + 50));

    
    Rect extracts[7] = {extRectOne, extRectTwo, extRectThree, extRectFour,
                        extRectFive, extRectSix, extRectSeven};

    namedWindow("binaryHand");
    createTrackbar("Threshold", "binaryHand", &binaryThreshold, 100);
    
    Scalar boxColour(0,255,0);
    
    while (true)
    {
        Mat frame;
        
        cap >> frame;
        
        frameCounter++;
        
        clock_t start, end;
        start = clock();
        
        skinFilter.importFrameWithContrast(frame);
        
        if (frameCounter < 100)
        {
           skinFilter.showExtractAreas(frame, extracts, boxColour);
            rectangle(frame, extRectEight, Scalar(0,255,0));
        }
        
        if (frameCounter == 100)
        {
            skinFilter.runExtractCollection(extracts);
            
            skinFilter.runColourCollection(binaryThreshold);
            
            boxColour = Scalar(0,0,255);
        }
        
        if (frameCounter >= 100)
        {
            Mat resizedFrame;
            
            vector<vector<Vec4i>> failedDefects;
            
            resize(frame, resizedFrame, Size(320,240), INTER_NEAREST);
        
            Mat binaryImage = skinFilter.runBinaryFiltering(resizedFrame);
            
            int topNContours = 2;
            
            handElements.findHandProperties(binaryImage, handContours, handHullPoints,
                                            handHullInts, handDefects, topNContours);
            
            
            handElements.dismissIrrelevantInfo(handDefects, handContours, 90, fingerAngles);
            
            failedDefects = handElements.getFailedDefects();
            
            bool expanded = isHandExpanded(fingerAngles, 50);
            Scalar handColour;
            
            if (expanded)
            {
                handColour = Scalar(0,255,0);
                cout << "PASSED - GREEN" << endl;
            }
            else
            {
                handColour = Scalar(0,0,255);
                cout << "FAILED - RED" << endl;
            }
            
            Mat handPositionId = Mat(binaryImage.rows, binaryImage.cols, CV_8UC3);
            handPositionId = handColour;
            
            Mat defectsImg = handElements.drawDefects(binaryImage, handDefects, handContours);
            
            Mat failedDefImg = handElements.drawDefects(binaryImage, failedDefects, handContours);
            
            imshow("binaryHand", binaryImage);
            imshow("defects", defectsImg);
            imshow("handColours", handPositionId);
            continue;
        }
        
        end = clock();
        double speedPerSec = double(end - start) / double(CLOCKS_PER_SEC);
        
        if (frameCounter % 10 == 0)
            cout << "Speed of program: " << double(speedPerSec) << " second(s)" << endl;
        
        if(waitKey(30) > 0)
            return -1;
        
        imshow("main", frame);
    }
    
    return 0;
}

bool isHandExpanded(vector<int> fingerAngles, int threshDegree)
{
    bool checkExpanded;
    int threshCount = 0;
    
    for (int angle : fingerAngles)
    {
        if (angle >= threshDegree) threshCount++;
    }
    
    checkExpanded = threshCount >= 4 ? true : false;
    
    return checkExpanded;
}

bool isFingerPointed(vector<vector<Vec4i>> defects, vector<vector<Point>> contours)
{
    return true;
}

bool checkPointedFinger(vector<vector<Vec4i>> defects, vector<vector<Point>> contours, vector<int> fingerAngles)
{
    return true;
}
















