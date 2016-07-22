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

#include <iostream>
#include <string.h>
#include <sstream>
#include <fstream>
#include <map>

using namespace cv;
using namespace std;

int BskinMin = 130, GskinMin = 160, RskinMin = 230;
int BskinMax = 200, GskinMax = 220, RskinMax = 255;

string file_loc = "/Users/dannyg/Desktop/Projects/HandTracking/HandTracking/Resources/";
string face_front = "/Users/dannyg/Desktop/haarcascade_frontalface_alt.xml";

CascadeClassifier faceCascade;
vector<Rect> faces;

bool withinRange(int val, int min, int max);

void findPrimaryColour(Mat frame, map<Vec3b, int> &outputChannelValues);
void extractColour(Mat frame);
void applySkinThresh(Mat frame);

Vec3b captureSkinColour(Mat frame);

void detectFaces(Mat frame, vector<Rect> &outputFaces);

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

vector<Mat3b> extractOneVec, extractTwoVec,
              extractThreeVec, extractFourVec;

Rect extractOne, extractTwo, extractThree, extractFour;

int main(int argc, const char * argv[]) {
    
    VideoCapture cap(0); //always BGR
    uint frameCounter = -1;
    
    if(!cap.isOpened())
        return -1;
    
    namedWindow("main");
    
    while (true)
    {
        Mat frame;
        
        cap >> frame;
        frameCounter++;
        
        int wCenter = frame.size().width / 2;
        int hCenter = frame.size().height / 2;
        
        if (frameCounter <= 100)
        {
            extractOne = Rect(Point(wCenter - 50, hCenter + 70), Point(wCenter - 30, hCenter + 50));
            extractTwo = Rect(Point(wCenter + 50, hCenter + 150), Point(wCenter + 30, hCenter + 130));
            extractThree = Rect(Point(wCenter + 50, hCenter + 70), Point(wCenter + 30, hCenter + 50));
            extractFour = Rect(Point(wCenter - 50, hCenter + 150), Point(wCenter - 30, hCenter + 130));
            
            extractColour(frame);
        }
        
        if (frameCounter >= 2)
        {
            applySkinThresh(frame);
            break;
        }
        
        
        imshow("main", frame);
        
        if(waitKey(30) > 0)
            return -1;
        
    }
    
    return 0;
}

void extractColour(Mat frame)
{
    rectangle(frame, extractOne,
                     Scalar(0,255,0));
    
    rectangle(frame, extractTwo,
                     Scalar(0,255,0));
    
    rectangle(frame, extractThree,
                     Scalar(0,255,0));
    
    rectangle(frame, extractFour,
                     Scalar(0,255,0)); //add center and one for each finger
    
    Mat3b colourOne = Mat3b(frame, extractOne),
    colourTwo = Mat3b(frame, extractTwo),
    colourThree = Mat3b(frame, extractThree),
    colourFour = Mat3b(frame, extractFour);
    
    extractOneVec.push_back(colourOne);
    extractTwoVec.push_back(colourTwo);
    extractThreeVec.push_back(colourThree);
    extractFourVec.push_back(colourFour);
}

void applySkinThresh(Mat frame)
{
    int pixelCount = frame.rows * frame.cols;
    
    int blueMode[255] = {0}, greenMode[255] = {0}, redMode[255] = {0};
    
    Vec3b channels;
 /*
    for (int colourVal = 0; colourVal <= 255; colourVal++)
    {
        for (int pixel = 0; pixel <= pixelCount; pixel++)
        {
            int b = pixel, g = pixel + 1, r = pixel + 2;
            
            channels = extractOneVec[0];
            
            cout << channels[0] << endl;
            
            //cout << extractOneVec[0].type() << endl;
            //cout << extractOneVec[1].type() << endl;
            //cout << extractOneVec[2].type() << endl;
            
            blueMode[2] += 1;
            
        }
    }*/
    
    for (int colourVal = 0; colourVal <= 255; colourVal++)
    {
        for(Mat pixels : extractOneVec)
        {
            cout << pixels.at<Vec3b>(0, 0) << endl;
            cout << colourVal << endl;
            
            /*
             TODO:
             get each pixel and add the intensity
             of each channel to modeColours above
             */
            
        }
    }
 
}

void detectFaces(Mat frame, vector<Rect> &outputFaces)
{
    cvtColor(frame, frame, CV_BGR2GRAY);
    equalizeHist(frame, frame);
    
    faceCascade.detectMultiScale(frame, outputFaces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30,30));
}

bool withinRange(int val, int min, int max) //efficiently checks whether val is in between min and max
{
    return (unsigned)(val - min) <= (max - min);
}
























