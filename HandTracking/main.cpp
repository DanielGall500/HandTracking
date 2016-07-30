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
        
        /*
         Speed of before optimization:
         Speed of program: 0.379313 second(s)
         Speed of program: 0.304982 second(s)
         Speed of program: 0.306485 second(s)
         Speed of program: 0.306367 second(s)
         Speed of program: 0.294823 second(s)
         Speed of program: 0.308851 second(s)
         Speed of program: 0.29538 second(s)
         Speed of program: 0.299404 second(s)
         Speed of program: 0.295455 second(s)
         Speed of program: 0.304174 second(s)
         Speed of program: 0.308579 second(s)
         Speed of program: 0.309152 second(s)
         Speed of program: 0.306726 second(s)
         Speed of program: 0.298693 second(s)
         Speed of program: 0.297937 second(s)
         Speed of program: 0.314955 second(s)
         */
        
        if (frameCounter == 48)
        {
            cout << "Extracting Colour" << endl;
            
            skinFilter.runExtractCollection(extracts);
            
            skinFilter.runColourCollection(threshold);
            
            boxColour = Scalar(0,0,255);
        }
        
        if (frameCounter >= 50)
        {
            //Mat resizedFrame;
            //resize(frame, resizedFrame, Size(320,240));
        
            Mat filtImg = skinFilter.runBinaryFiltering();
            
            imshow("Filtered", filtImg);
            
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






















