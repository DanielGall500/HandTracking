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
map<Vec3b, int> channelValues;

bool withinRange(int val, int min, int max);

void findPrimaryColour(Mat frame, map<Vec3b, int> &outputChannelValues);
void extractColour(Mat frame);
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

Rect extractOne, extractTwo, extractThree, extractFour;

int main(int argc, const char * argv[]) {
    
    VideoCapture cap(0); //always BGR
    uint frameCounter = -1;
    
    if(!cap.isOpened())
        return -1;
    
    faceCascade.load(face_front);
    
    if(!faceCascade.load(face_front))
    {
        cout << "Error loading cascade" << endl;
    }
    
    namedWindow("main");
    
    while (true)
    {
        Mat frame;
        
        cap >> frame;
        frameCounter++;
        
        int wCenter = frame.size().width / 2;
        int hCenter = frame.size().height / 2;
        
        if (frameCounter <= 500)
        {
            extractOne = Rect(Point(wCenter - 50, hCenter + 70), Point(wCenter - 30, hCenter + 50));
            extractTwo = Rect(Point(wCenter + 50, hCenter + 150), Point(wCenter + 30, hCenter + 130));
            extractThree = Rect(Point(wCenter + 50, hCenter + 70), Point(wCenter + 30, hCenter + 50));
            extractFour = Rect(Point(wCenter - 50, hCenter + 150), Point(wCenter - 30, hCenter + 130));
            
            extractColour(frame);
        }
        
        if(!frame.empty())
        {
            
        }
        else
        {
            cout << "Empty frame: " << endl;
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
    
    Mat colourOne = Mat(frame, extractOne);
    Mat colourTwo = Mat(frame, extractTwo);
    Mat colourThree = Mat(frame, extractThree);
    Mat colourFour = Mat(frame, extractFour);
    
    for (int row = 0; row <= frame.rows; row++)
    {
        for(int col = 0; col <= frame.cols; col++)
        {
            Vec3b channelsOne = colourOne.at<Vec3b>(row, col),
                  channelsTwo = colourTwo.at<Vec3b>(row, col),
                  channelsThree = colourThree.at<Vec3b>(row, col),
                  channelsFour = colourFour.at<Vec3b>(row, col);
            
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


/*
void findPrimaryColour(Mat frame, map<Vec3b, int> &outputChannelValues)
{
    Vec3b colChannels;
    int b, g, r;
    
    for (int rows = 0; rows <= frame.rows; rows++)
    {
        for (int cols = 0; cols <= frame.cols; cols++)
        {
            
            colChannels = frame.at<Vec3i>(rows, cols);
            
            b = colChannels[0];
            g = colChannels[1];
            r = colChannels[2];
            
            colChannels[2] += 100;
            
            
            if (outputChannelValues.count(colChannels) == 0)
                outputChannelValues[colChannels] = 1;
            else
                outputChannelValues[colChannels] += 1;
        
            cout << colChannels << ": " << outputChannelValues[colChannels] << endl;
            
            
            frame.at<Vec3b>(rows, cols) = colChannels;
             
        }
    }
}
*/


























