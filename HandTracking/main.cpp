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

bool withinRange(int val, int min, int max);
int findChannelMax(int colours[]);

void extractColour(Mat frame);
void showColourExtracts(Mat frame);

Vec3b extractBinaryFilter(Mat extractFrame, Mat originalFrame);
Mat skinBinaryFilter(Mat originalFrame, int threshold);

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

Vec3b testFilterPix;
int thresh = 10;

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
        
   
        extractOne = Rect(Point(wCenter - 50, hCenter + 70), Point(wCenter - 30, hCenter + 50));
        extractTwo = Rect(Point(wCenter + 50, hCenter + 150), Point(wCenter + 30, hCenter + 130));
        extractThree = Rect(Point(wCenter + 50, hCenter + 70), Point(wCenter + 30, hCenter + 50));
        extractFour = Rect(Point(wCenter - 50, hCenter + 150), Point(wCenter - 30, hCenter + 130));
        
        showColourExtracts(frame);

        
        if (frameCounter == 48)
        {
            extractColour(frame);
            
            testFilterPix = extractBinaryFilter(extractOneVec[0], frame);
        }
        
        if (frameCounter >= 50)
        {
            Mat filteredImage = skinBinaryFilter(frame, thresh);
            
            Mat blurredImg;
            
            medianBlur(filteredImage, blurredImg, 7);
            
            imshow("Filtered", blurredImg);
        }
        
        if(waitKey(30) > 0)
            return -1;
        
        imshow("main", frame);
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

Vec3b extractBinaryFilter(Mat extractFrame, Mat originalFrame)
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
        }
    }
    
    int blueMode = findChannelMax(blueCollection),
        greenMode = findChannelMax(greenCollection),
        redMode = findChannelMax(redCollection);
    
    cout << "B Mode: " << blueMode << endl;
    cout << "G Mode: " << greenMode << endl;
    cout << "R Mode: " << redMode << endl;
    
    Vec3b filterPixel = Vec3b(blueMode, greenMode, redMode);
    
    return filterPixel;
    
}

Mat skinBinaryFilter(Mat originalFrame, int threshold)
{
    Mat outputImage(originalFrame.size(), CV_8UC3, Scalar(0,0,0));
    
    for (int row = 0; row <= originalFrame.rows; row++) //probably a better method for this
    {
        for (int col = 0; col <= originalFrame.cols; col++)
        {
            Vec3b originalPixel = originalFrame.at<Vec3b>(row, col);
            
            int b = originalPixel[0],
                g = originalPixel[1],
                r = originalPixel[2];
            
            int minBlueIntensity = testFilterPix[0] - threshold,
                maxBlueIntensity = testFilterPix[0] + threshold;
            
            int minGreenIntensity = testFilterPix[1] - threshold,
                maxGreenIntensity = testFilterPix[1] + threshold;
            
            int minRedIntensity = testFilterPix[2] - threshold,
                maxRedIntensity = testFilterPix[2] + threshold;

            if (withinRange(b, minBlueIntensity, maxBlueIntensity) &&
                withinRange(g, minGreenIntensity, maxGreenIntensity) &&
                withinRange(r, minRedIntensity, maxRedIntensity))
                outputImage.at<Vec3b>(row, col) = Vec3b(255,255,255);
        }
    }
    return outputImage;
}

bool withinRange(int val, int min, int max) //used for checking if image pixels are in certain threshold
{
    return (unsigned)(val - min) <= (max - min);
}

int findChannelMax(int colours[])
{
    int maxColourCount = 0;
    int maxIntensity = 0;
    
    for (short int intensity = 0; intensity <= 255; intensity++)
    {
        cout << "Intensity: " << intensity << endl;
        cout << "Occurrences: " << colours[intensity] << endl;
        
        if (colours[intensity] > maxColourCount &&
            intensity != 0 &&
            intensity != 255)
        {
            cout << "NEW MOSt-OCCURRING INTENSITY" << endl;
            cout << colours[intensity] << " is greater than " << maxColourCount << endl;
            cout << maxIntensity << " is now the most-occurring intensity" << endl;
            
            maxColourCount = colours[intensity];
            maxIntensity = intensity;
        }
    }
    
    return maxIntensity;
}
























