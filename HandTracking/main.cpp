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
    int threshold = 5;
    
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
        
        skinFilter.showExtractAreas(frame, extracts, boxColour);
        
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
            
            imshow("main", binaryImage);
            
            Mat contours = findFingers(binaryImage);
            
            imshow("main", contours);
            
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

Mat findFingers(Mat binaryHand)
{
    cvtColor(binaryHand, binaryHand, CV_BGR2GRAY);
    
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    
    findContours(binaryHand, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    
    Mat contourImg = Mat::zeros(binaryHand.rows, binaryHand.cols, CV_8UC3);
    
    for (int i = 0; i < contours.size(); i++)
    {
        drawContours(contourImg, contours, i, Scalar(0,255,0));
    }
    
    return contourImg;
}
























