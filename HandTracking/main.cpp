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
int findChannelMax(int colours[]);

void findPrimaryColour(Mat frame, map<Vec3b, int> &outputChannelValues);
void extractColour(Mat frame);
void showColourExtracts(Mat frame);
void skinBinaryFilter(Mat extractFrame, Mat originalFrame);

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

vector<Mat> extractOneVec, extractTwoVec,
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
        
        if (frameCounter <= 48)
        {
            extractOne = Rect(Point(wCenter - 50, hCenter + 70), Point(wCenter - 30, hCenter + 50));
            extractTwo = Rect(Point(wCenter + 50, hCenter + 150), Point(wCenter + 30, hCenter + 130));
            extractThree = Rect(Point(wCenter + 50, hCenter + 70), Point(wCenter + 30, hCenter + 50));
            extractFour = Rect(Point(wCenter - 50, hCenter + 150), Point(wCenter - 30, hCenter + 130));
            
            showColourExtracts(frame);
        }
        else
        {
            extractColour(frame);
            
            skinBinaryFilter(extractOneVec[0], frame);
            
            terminate();
        }
        
        
        imshow("main", frame);
        
        if(waitKey(30) > 0)
            return -1;
        
    }
    
    return 0;
}

void showColourExtracts(Mat frame)
{
    rectangle(frame, extractOne,
              Scalar(0,255,0));
    
    rectangle(frame, extractTwo,
              Scalar(0,255,0));
    
    rectangle(frame, extractThree,
              Scalar(0,255,0));
    
    rectangle(frame, extractFour,
              Scalar(0,255,0)); //add center and one for each finger
}

void extractColour(Mat frame)
{
    Mat colourOne = Mat(frame, extractOne),
    colourTwo = Mat(frame, extractTwo),
    colourThree = Mat(frame, extractThree),
    colourFour = Mat(frame, extractFour);
    
    extractOneVec.push_back(colourOne);
    extractTwoVec.push_back(colourTwo);
    extractThreeVec.push_back(colourThree);
    extractFourVec.push_back(colourFour);
}

void skinBinaryFilter(Mat extractFrame, Mat originalFrame)
{
    int blueCollection[255] = {0},
        greenCollection[255] = {0},
        redCollection[255] = {0};
    
    Vec3b channels;
    
    for (short int row = 0; row <= extractFrame.rows; row++)
    {
        for (short int col = 0; col <= extractFrame.cols; col++)
        {
            channels = extractFrame.at<Vec3b>(row, col);
           
            int b = channels[0],
                g = channels[1],
                r = channels[2];
            
            blueCollection[b] += 1;
            greenCollection[g] += 1;
            redCollection[r] += 1;
            
            cout << b << endl;
            cout << g << endl;
            cout << r << endl;
            cout << endl;
        }
    }
    
    int blueMode = findChannelMax(blueCollection),
        greenMode = findChannelMax(greenCollection),
        redMode = findChannelMax(redCollection);
    
    cout << "B Mode: " << blueMode << endl;
    cout << "G Mode: " << greenMode << endl;
    cout << "R Mode: " << redMode << endl;
    
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

int findChannelMax(int colours[])
{
    int maxColourCount = 0;
    int maxIntensity = 0;
    
    for (short int intensity = 0; intensity <= 255; intensity++)
    {
        if (colours[intensity] > maxColourCount)
        {
            maxColourCount = colours[intensity];
            maxIntensity = intensity;
        }
    }
    
    return maxIntensity;
}
























