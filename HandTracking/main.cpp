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

#include <iostream>
#include <string.h>
#include <sstream>
#include <fstream>
#include <time.h>

using namespace cv;
using namespace std;

Mat findFingers(Mat binaryHand);
void findHandProperties(Mat binaryHand,
                        vector<vector<Point>> &outputContours,
                        vector<vector<Point>> &outputHull,
                        vector<vector<int>> &hullInts,
                        vector<vector<Vec4i>> &outputDefects);

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

int main(int argc, const char * argv[]) {
    
    VideoCapture cap(0); //always BGR
    uint frameCounter = -1;
    
    if(!cap.isOpened())
        return -1;
    
    BinarySkinFilter skinFilter;
    int threshold = 25;
    
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
            
            vector<vector<Point>> handContours;
            vector<vector<Point>> handHullPoints;
            vector<vector<int>> handHullInts;
            vector<vector<Vec4i>> handDefects;
            
            findHandProperties(binaryImage, handContours, handHullPoints, handHullInts, handDefects);
            
            imshow("enclosedHand", binaryImage);
            
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

void findHandProperties(Mat binaryImage,
                        vector<vector<Point>> &outputContours,
                        vector<vector<Point>> &outputHull,
                        vector<vector<int>> &outputHullInts,
                        vector<vector<Vec4i>> &outputDefects)
{
    Mat editedImg;
    Mat handContourImg = Mat::zeros(binaryImage.rows, binaryImage.cols, CV_8UC3),
    handConvexityDefects = Mat::zeros(binaryImage.rows, binaryImage.cols, CV_8UC3);
    
    cvtColor(binaryImage, editedImg, CV_BGR2GRAY);
    
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    
    findContours(editedImg, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    
    vector<vector<Point>> hull(contours.size());
    vector<vector<int>> hullInts(contours.size());
    vector<vector<Vec4i>> defects(contours.size());
    
    for (int i = 0; i < contours.size(); i++)
    {
        convexHull(contours[i], hull[i], false);
        convexHull(contours[i], hullInts[i], false);
        
        if (hullInts[i].size() > 3)
        {
            convexityDefects(contours[i], hullInts[i], defects[i]);
        }
        
        drawContours(handContourImg, hull, i, Scalar(255,255,0));
    }
    
    //find largest
    int defIdx = 0;
    int maxArea = 0;
    
    for (int i = 0; i < contours.size(); i++)
    {
        double a = contourArea(contours[i]);
        if (a > maxArea)
        {
            maxArea = a;
            defIdx = i;
        }
    }
  
    for (int i = 0; i < defects[defIdx].size(); i++)
    {
        Vec4i& v = defects[defIdx][i];
        
        float depth = v[3] / 256;
        if (depth > 10)
        {
            int startIdx = v[0];
            Point ptStart(contours[defIdx][startIdx]);
            
            int farIdx = v[1];
            Point ptFar(contours[defIdx][farIdx]);
            
            int endIdx = v[2];
            Point ptEnd(contours[defIdx][endIdx]);
            
            Scalar defectCol(0,0,255);
            
            line(handConvexityDefects, ptStart, ptEnd, defectCol, 1);
            line(handConvexityDefects, ptStart, ptFar, defectCol, 1);
            line(handConvexityDefects, ptEnd, ptFar, defectCol, 1);
            circle(handConvexityDefects, ptFar, 4, defectCol, 2);
            
        }
    }
    outputContours = contours;
    outputHull = hull;
    outputHullInts = hullInts;
    outputDefects = defects;

}






















