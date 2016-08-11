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
#include <vector>
#include <algorithm>
#include <math.h>

using namespace cv;
using namespace std;

Mat findFingers(Mat binaryHand);
Mat drawDefects(Mat frame, vector<vector<Vec4i>> defects, vector<vector<Point>> contours);
vector<vector<Point>> getNMaxContours(vector<vector<Point>> contours, int n);

vector<vector<Vec4i>> dismissNonFingerDefects(vector<vector<Vec4i>> defects,
                                        vector<vector<Point>> contours,
                                        float angleThresh);

void findHandProperties(Mat binaryHand,
                        vector<vector<Point>> &outputContours,
                        vector<vector<Point>> &outputHull,
                        vector<vector<int>> &hullInts,
                        vector<vector<Vec4i>> &outputDefects,
                        int topNContours);

bool operator < (vector<Point> c1, vector<Point> c2)
{
    return contourArea(c1) < contourArea(c2);
}

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
    int threshold = 15;
    
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
            
            int topNContours = 2;
            
            findHandProperties(binaryImage, handContours, handHullPoints, handHullInts, handDefects, topNContours);
            
            vector<vector<Vec4i>> cleanDefects = dismissNonFingerDefects(handDefects, handContours, 80);
            
            Mat fullDefectsImg = drawDefects(binaryImage, handDefects, handContours);
            Mat cleanDefectsImg = drawDefects(binaryImage, cleanDefects, handContours);
            
            imshow("enclosedHand", binaryImage);
            imshow("defects", fullDefectsImg);
            imshow("cleaned defects", cleanDefectsImg);
            
            continue;
        }
        
        end = clock();
        double speedPerSec = double(end - start) / double(CLOCKS_PER_SEC);
        
        if (frameCounter % 10 == 0)
            cout << "Speed of program: " << double(speedPerSec) << " second(s)" << endl;
        
        if(waitKey(30) > 0)
            return -1;
        
       // imshow("main", frame);
    }
    
    return 0;
}

void findHandProperties(Mat binaryImage,
                        vector<vector<Point>> &outputContours,
                        vector<vector<Point>> &outputHull,
                        vector<vector<int>> &outputHullInts,
                        vector<vector<Vec4i>> &outputDefects,
                        int topNContours)
{
    Mat editedImg;
    Mat handContourImg = Mat::zeros(binaryImage.rows, binaryImage.cols, CV_8UC3);
    
    cvtColor(binaryImage, editedImg, CV_BGR2GRAY);
    
    vector<vector<Point>> contours;
    vector<vector<Point>> maxContours;
    vector<Vec4i> hierarchy;
    
    int numMaxContours = 1;
    
    findContours(editedImg, contours, hierarchy, CV_RETR_TREE,
                 CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    
    maxContours = getNMaxContours(contours, numMaxContours);
    
    vector<vector<Point>> hull(maxContours.size());
    vector<vector<int>> hullInts(maxContours.size());
    vector<vector<Vec4i>> defects(maxContours.size());
    
    for (int i = 0; i < maxContours.size(); i++)
    {
        convexHull(maxContours[i], hull[i], false);
        convexHull(maxContours[i], hullInts[i], false);
        
        if (hullInts[i].size() > 3)
        {
            convexityDefects(maxContours[i], hullInts[i], defects[i]);
        }
        
        drawContours(handContourImg, hull, i, Scalar(255,255,0));
    }
    
    outputContours = maxContours;
    outputHull = hull;
    outputHullInts = hullInts;
    outputDefects = defects;

}

vector<vector<Point>> getNMaxContours(vector<vector<Point>> contours, int n)
{
    vector<vector<Point>> nContours;
    vector<double> contourAreas;
    
    for (int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        contourAreas.push_back(area);
    }
    
    for (int i = 0; i < n; i++)
    {
        vector<double>::iterator result;
        
        result = max_element(contourAreas.begin(), contourAreas.end());
        
        float position = std::distance(contourAreas.begin(), result);
        
        nContours.push_back(contours[position]);
        contours.erase(contours.begin() + position);
    }
    
    return nContours;
}

Mat drawDefects(Mat frame, vector<vector<Vec4i>> defects, vector<vector<Point>> contours)
{
    Mat handConvexityDefects = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
    
    for (int defIdx = 0; defIdx < contours.size(); defIdx++)
    {
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
                
                line(handConvexityDefects, ptStart, ptEnd, Scalar(255,0,0), 1); //blue
                line(handConvexityDefects, ptStart, ptFar, Scalar(0,255,0), 1); //green
                line(handConvexityDefects, ptEnd, ptFar, Scalar(0,0,255), 1); //red
                circle(handConvexityDefects, ptFar, 4, Scalar(255,255,255), 2); //white
                
            }
        }
    }
    
    return handConvexityDefects;
}

vector<vector<Vec4i>> dismissNonFingerDefects(vector<vector<Vec4i>> defects, vector<vector<Point>> contours, float angleThresh)
{
    vector<vector<Vec4i>> passedDefects;
    
    auto findSlope = [] (Point pt1, Point pt2)
    {
        float slope;
        
        int yDist = pt2.y - pt1.y;
        int xDist = pt2.x - pt1.x;
        
        // make sure we don't divide by zero
        // for the slope as makes hardware implode
        
        if (xDist == 0 || yDist == 0)
            slope = 0;
        else
            slope = yDist / xDist;
        
        return slope;
    };
    
    for (int idx = 0; idx < contours.size(); idx++)
    {
        cout << "Idx: " << idx << endl;
        
        vector<Vec4i> subDefects;
        
        for (int i = 0; i < defects[idx].size(); i++)
        {
            Vec4i defect = defects[idx][i];
            
            cout << "i: " << i << endl;
            
            int startIdx = defect[0],
                farIdx = defect[1],
                endIdx = defect[2];
            
            cout << "StartIdx: " << startIdx << endl;
            cout << "EndIdx: " << endIdx << endl;
            cout << "FarIdx: " << farIdx << endl;
            
            Point ptStart(contours[idx][startIdx]),
                  ptFar(contours[idx][farIdx]),
                  ptEnd(contours[idx][endIdx]);
            
            float startEndSlp = findSlope(ptStart, ptEnd), //blue slope 1
                  startFarSlp = findSlope(ptStart, ptFar), //green
                  endFarSlp = findSlope(ptEnd, ptFar); //red slope 2
            
            float commonAngle = atan((startEndSlp + endFarSlp) / (1 - startEndSlp * endFarSlp)); //find angle between two slopes
            
            if (commonAngle <= angleThresh)
                subDefects.push_back(defect);
        }
        passedDefects.push_back(subDefects);
    }
    
    return passedDefects;
}

























