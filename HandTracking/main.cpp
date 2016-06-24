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

using namespace cv;
using namespace std;

int BskinMin = 130, GskinMin = 160, RskinMin = 230;
int BskinMax = 200, GskinMax = 220, RskinMax = 255;

string file_loc = "/Users/dannyg/Desktop/Projects/HandTracking/HandTracking/Resources/";
string face_front = "/Users/dannyg/Desktop/haarcascade_frontalface_alt.xml";
string skin_loc = "/Users/dannyg/Desktop/Projects/Hand\ Tracking/HandTracking/Resources/SkinColour.jpg";

CascadeClassifier faceCascade;
vector<Rect> faces;
Mat skinColour;

bool withinRange(int val, int min, int max);

Mat findPrimaryColour(Mat frame);
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

int main(int argc, const char * argv[]) {
    
    VideoCapture cap(0); //always BGR
    uint frameCounter = -1;
    
    if(!cap.isOpened())
        return -1;
    
    faceCascade.load(face_front);
    
    skinColour = imread(skin_loc);
    cvtColor(skinColour, skinColour, CV_RGB2BGR);
    
    if(!faceCascade.load(face_front))
    {
        cout << "Error loading cascade" << endl;
    }
    
    while (true)
    {
        Mat frame;
        
        cap >> frame;
        frameCounter++;
        
        if (frameCounter <= 24)
        {
            detectFaces(frame, faces);
            
            for (int i = 0; i < faces.size(); i++)
            {
                Point center(faces[i].x + faces[i].width * 0.5, faces[i].y + faces[i].height * 0.5);
                
                ellipse(frame, center, Size(faces[i].width * 0.5, faces[i].height * 0.5),
                        0, 0, 360, Scalar(0,255,0), 4, 8, 0);
            }
        }
        
        if(!frame.empty())
        {
            
        }
        else
        {
            cout << "Empty frame: " << endl;
        }
        
        //GaussianBlur(frame, blurred, Size(7,7), 1.5);
        
        //Mat newFrame = findSkin(blurred);
        
        //cout << newFrame.at<Vec3b>(0,0) << endl;
        
        imshow("Testing", frame);
        
        if(waitKey(30) > 0)
            return -1;
        
    }
    
    return 0;
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

Mat findPrimaryColour(Mat frame)
{
    Vec3b colChannels;
    int b, g, r;
    
    for (int rows = 0; rows <= frame.rows; rows++)
    {
        for (int cols = 0; cols <= frame.cols; cols++)
        {
            colChannels = frame.at<Vec3b>(rows, cols);
            
            b = colChannels[0];
            g = colChannels[1];
            r = colChannels[2];
            
            
            
            frame.at<Vec3b>(rows, cols) = colChannels;
            
        }
    }
    return frame;
}











