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
#include <map>

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
    
    while (true)
    {
        Mat frame;
        
        cap >> frame;
        
        frameCounter++;

        rectangle(frame, Point(wCenter - 10, hCenter - 10), Point(wCenter + 10, hCenter + 10), Scalar(255,0,0));
        
        skinFilter.updateFrame(frame);
        
        skinFilter.showExtractAreas(frame, extracts);

        
        if (frameCounter == 48)
        {
            cout << "Running Colour Collection" << endl;
            
            skinFilter.runExtractCollection(extracts);
            
            skinFilter.runColourCollection();
        }
        
        if (frameCounter >= 50)
        {
            cout << "Frame >50" << endl;
            
            skinFilter.runBinaryFiltering(frame, threshold);
        
            vector<Mat> binaryImgs = skinFilter.getBinaryImages();
            
            Mat summedBinaryImg = skinFilter.getSummedBinaryImages();
            
            imshow("Filtered", summedBinaryImg);
        }
        
        if(waitKey(30) > 0)
            return -1;
        
        imshow("main", frame);
    }
    
    return 0;
}






















