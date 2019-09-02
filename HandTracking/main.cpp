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

//bool isHandExpanded(vector<int> fingerAngles, int threshDegree);

const int numExtracts = 8;
Rect extracts[numExtracts];

//Rect extRectOne, extRectTwo, extRectThree, extRectFour,
 //    extRectFive, extRectSix, extRectSeven, extRectEight;

vector<vector<Point>> handContours;
vector<vector<Point>> handHullPoints;
vector<vector<int>> handHullInts;
vector<vector<Vec4i>> handDefects;

vector<int> fingerAngles;

HandElementContainer ptManager;

int contrastMultiplier = 2;
int binaryThreshold = 15;
int colourCountForExtracts = 3;

bool SHOW_DEFECTS = false;

vector<Vec3b> filtColours;

void read_images(std::string imgs_filename, vector<Mat> &images, vector<int> labels, char seperator = ';');

int main(int argc, const char * argv[]) {
    
    VideoCapture cap(0); //always BGR
    
    int frameCounter = -1;
    
    if(!cap.isOpened())
        return -1;
    
    BinarySkinFilter skinFilter;
    
    HandElementContainer handElements;
    
    Mat firstFrame;
    cap >> firstFrame;
    
    //Find the centre of the images
    int wCentre = firstFrame.size().width / 2;
    int hCentre = firstFrame.size().height / 2;
    
    /* CREATE EXTRACTS
       These will be rectangles within the frame that will
       be used to align our hand and find it's dominant colours*/
    
    int R1[8][2] = {
        {-100, +70}, {-100, +150},
        {-100, +10}, {-120, -240},
        {-70,  -280}, {+70,  -280},
        {-10,  -310}, {+140, -50}
    };
    
    int R2[8][2] {
        {+100, +50}, {+100, +130},
        {+100, -10}, {-100, -80},
        {-50,  -80}, {+50,  -80},
        {+10,  -110},{+120, +0}
    };
    
    for(int i = 0; i < numExtracts; i++)
    {
        Point p1 { wCentre + R1[i][0],
                   hCentre + R1[i][1] };
        
        Point p2 { wCentre + R2[i][0],
                   hCentre + R2[i][1] };
        
        extracts[i] = Rect(p1, p2);
    }

    namedWindow("binaryHand");
    
    Scalar boxColour(0,255,0);
    
    while (true)
    {
        //Create a matrix containing each frame
        Mat frame;
        
        cap >> frame;
        
        frameCounter++;
        
        clock_t start, end;
        start = clock();
        
        skinFilter.importFrameOriginal(frame);
        
        if (frameCounter < 100)
        {
           //Show the areas that pixels will be extracted from
           skinFilter.showExtractAreas(frame, extracts, boxColour);
        }
        else if (frameCounter == 100)
        {
            skinFilter.collectImageExtracts(extracts);
            
            skinFilter.runColourCollection(binaryThreshold, colourCountForExtracts);
            
            boxColour = Scalar(0,0,255);
            
            cout << "COLOURS" << endl;
            for(auto c : skinFilter.getFilterCols())
            {
                cout << c << endl;
            }
            
            filtColours = skinFilter.getFilterCols();
            
            cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
            cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
        }
        else //AFTER 100th FRAME
        {
            vector<vector<Vec4i>> failedDefects;
            
            //Binary Filtering
            Mat binaryImage = skinFilter.runBinaryFiltering(frame);
            
            
            if(SHOW_DEFECTS)
            {
                int topNContours = 2;
                
                handElements.findHandProperties(binaryImage, handContours, handHullPoints,
                                                handHullInts, handDefects, topNContours);
                
                
                handElements.dismissIrrelevantInfo(handDefects, handContours, 90, fingerAngles);
            
                failedDefects = handElements.getFailedDefects();
                
                
                //Draw the defects i.e the lines connecting different parts of the hand
                Mat defectsImg = handElements.drawDefects(binaryImage, handDefects, handContours);
                
                Mat failedDefImg = handElements.drawDefects(binaryImage, failedDefects, handContours);
                
                imshow("defects", defectsImg);
            }
            
            //Show the image of the binary hand
            imshow("binaryHand", binaryImage);
            
            //Display a filter colour
            Vec3b col_one = filtColours[0];
            Mat matColOne = Mat(50,50, CV_8UC3, col_one);
            imshow("Filter Colours", matColOne);
            
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

void read_images(std::string imgs_filename, vector<Mat> &images, vector<int> labels, char seperator)
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

/*
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
}*/


















