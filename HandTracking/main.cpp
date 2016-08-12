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

Rect extractRectOne, extractRectTwo, extractRectThree, extractRectFour,
    extractRectFive, extractRectSix, extractRectSeven;

vector<vector<Point>> handContours;
vector<vector<Point>> handHullPoints;
vector<vector<int>> handHullInts;
vector<vector<Vec4i>> handDefects;

int main(int argc, const char * argv[]) {
    
    VideoCapture cap(0); //always BGR
    uint frameCounter = -1;
    
    if(!cap.isOpened())
        return -1;
    
    BinarySkinFilter skinFilter;
    int threshold = 15;
    
    HandElementContainer handElements;
    
    Mat firstFrame;
    cap >> firstFrame;
    
    int wCenter = firstFrame.size().width / 2;
    int hCenter = firstFrame.size().height / 2;
    
    extractRectOne = Rect(Point(wCenter - 50, hCenter + 70), Point(wCenter - 30, hCenter + 50));
    extractRectTwo = Rect(Point(wCenter + 50, hCenter + 150), Point(wCenter + 30, hCenter + 130));
    extractRectThree = Rect(Point(wCenter + 50, hCenter + 70), Point(wCenter + 30, hCenter + 50));
    extractRectFour = Rect(Point(wCenter - 50, hCenter + 150), Point(wCenter - 30, hCenter + 130));
    
    extractRectFive = Rect(Point(wCenter - 70, hCenter - 120), Point(wCenter - 50, hCenter - 100));
    extractRectSix = Rect(Point(wCenter + 70, hCenter - 120), Point(wCenter + 50, hCenter - 100));
    extractRectSeven = Rect(Point(wCenter - 10, hCenter - 150), Point(wCenter + 10, hCenter - 130));

    
    Rect extracts[7] = {extractRectOne, extractRectTwo, extractRectThree, extractRectFour,
                        extractRectFive, extractRectSix, extractRectSeven};

    namedWindow("main");
    
    Scalar boxColour(0,255,0);
    
    while (true)
    {
        Mat frame;
        
        cap >> frame;
        
        frameCounter++;
        
        clock_t start, end;
        start = clock();
        
        skinFilter.updateFrame(frame);
        
        if (frameCounter < 48)
        {
           skinFilter.showExtractAreas(frame, extracts, boxColour);
        }
        
        if (frameCounter == 48)
        {
            skinFilter.runExtractCollection(extracts);
            
            skinFilter.runColourCollection(threshold);
            
            boxColour = Scalar(0,0,255);
        }
        
        if (frameCounter >= 50)
        {
            Mat resizedFrame;
            resize(frame, resizedFrame, Size(320,240), INTER_NEAREST);
        
            Mat binaryImage = skinFilter.runBinaryFiltering(resizedFrame);
            
            int topNContours = 2;
            
            handElements.findHandProperties(binaryImage, handContours, handHullPoints,
                                            handHullInts, handDefects, topNContours);
            
            handElements.dismissIrrelevantDefects(handDefects, handContours, 90);
            
            Mat defectsImg = handElements.drawDefects(binaryImage, handDefects, handContours);
            
            imshow("enclosedHand", binaryImage);
            imshow("defects", defectsImg);
            
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


















