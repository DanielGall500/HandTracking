//
//  BinarySkinFilter.cpp
//  HandTracking
//
//  Created by Daniel Gallagher on 24/07/2016.
//  Copyright (c) 2016 Developer Co. All rights reserved.
//

#include "BinarySkinFilter.h"

using namespace cv;
using namespace std;

BinarySkinFilter::BinarySkinFilter()
{
}

BinarySkinFilter::~BinarySkinFilter()
{
}

void BinarySkinFilter::runColourCollection(int filterThreshold, int dominantColours)
{
    vector<Vec3b> cols;
    
    for(int ext = 0; ext < extractFrameStorage.size(); ext++)
    {
        cols = findDominantColours(extractFrameStorage[ext], imgMultiplier, dominantColours);
        
        cout << "EXTRACT " << ext << endl;
        
        for (Vec3b filterColour : cols)
        {
            cout << "Filter: " << filterColour << endl;
            filterColours.push_back(filterColour);
        }
    }
    
    findMinMaxChannels(filterColours, filterThreshold, minB, maxB, minG, maxG, minR, maxR);
}

vector<Vec3b> BinarySkinFilter::findDominantColours(Mat extractFrame, int multiplier, int amountOfColours)
{
    int blueCollection[255] = {0},
        greenCollection[255] = {0},
        redCollection[255] = {0};
    
    Vec3b channels;
    Vec3b filterPixel;
    
    vector<Vec3b> filterPixels;
    
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
    
    for (int i = 0; i < amountOfColours; i++)
    {
        int blueMode = findChannelMax(blueCollection),
        greenMode = findChannelMax(greenCollection),
        redMode = findChannelMax(redCollection);
        
        filterPixel = Vec3b(blueMode, greenMode, redMode);
        
        filterPixels.push_back(filterPixel);
        
        blueCollection[blueMode] = -1;
        greenCollection[greenMode] = -1;
        redCollection[redMode] = -1;
    }
    
    return filterPixels;
}

Mat BinarySkinFilter::runBinaryFiltering(Mat frame)
{
    Mat outputImage(frame.size(), CV_8UC3, Scalar(0,0,0));
    Vec3b *rowPixel;

    for (int row = 0; row <= frame.rows; row++)
    {
        rowPixel = frame.ptr<Vec3b>(row);
    
        for (int col = 0; col <= frame.cols; col++)
        {
            int b = rowPixel[col][0],
                g = rowPixel[col][1],
                r = rowPixel[col][2];
            
            if (withinFilterRange(b, g, r))
                outputImage.at<Vec3b>(row, col) = Vec3b(255,255,255);
        }
    }
    
    medianBlur(outputImage, outputImage, 7);
    
    return outputImage;
}

void BinarySkinFilter::findMinMaxChannels(vector<Vec3b> filter, int thresh,
                                          vector<int> &minB, vector<int> &maxB,
                                          vector<int> &minG, vector<int> &maxG,
                                          vector<int> &minR, vector<int> &maxR)
{
    for (int i = 0; i <= filter.size() - 1; i++)
    {
        minB.push_back(filter[i][0] - thresh),
        maxB.push_back(filter[i][0] + thresh);
        
        minG.push_back(filter[i][1] - thresh),
        maxG.push_back(filter[i][1] + thresh);
        
        minR.push_back(filter[i][2] - thresh),
        maxR.push_back(filter[i][2] + thresh);
    }
}

int BinarySkinFilter::findChannelMax(int colours[])
{
    int maxColourCount = 0;
    int maxIntensity = 0;
    
    for (short int intensity = 0; intensity <= 255; intensity++)
    {
        if (colours[intensity] > maxColourCount &&
            intensity != 0 &&
            intensity != 255)
        {
            maxColourCount = colours[intensity];
            maxIntensity = intensity;
        }
    }
    return maxIntensity;
}

bool BinarySkinFilter::withinFilterRange(int b, int g, int r) //need to comment this badly
{
    bool bPassed = false, gPassed = false, rPassed = false;
    int f = 0;
    
    auto channelInsideRange = [](int channel, int min, int max, bool passedCheck)
    {
        if (!passedCheck)
            return ((unsigned)(channel - min) <= (max - min));
        else
            return true;
    };
    
    while ((!bPassed && !gPassed && !rPassed) && f <= filterColours.size())
    {
        bPassed = channelInsideRange(b, minB[f], maxB[f], bPassed);
        gPassed = channelInsideRange(g, minG[f], maxB[f], gPassed);
        rPassed = channelInsideRange(r, minR[f], maxR[f], rPassed);
        
        f++;
    }
    return (bPassed && gPassed && rPassed);
}

void BinarySkinFilter::setExtractLocations(Rect extractRects[totalExtracts])
{
    extractRectOne = extractRects[0]; //need to be kept seperate
    extractRectTwo = extractRects[1];
    extractRectThree = extractRects[2];
    extractRectFour = extractRects[3];
    extractRectFive = extractRects[4];
    extractRectSix = extractRects[5];
    extractRectSeven = extractRects[6];
    extractRectEight = extractRects[7];
}


void BinarySkinFilter::showExtractAreas(Mat frame, Rect extracts[totalExtracts], Scalar colour)
{
    for (int i = 0; i < totalExtracts; i++)
    {
        rectangle(frame, extracts[i], colour);
    }
}

void BinarySkinFilter::collectImageExtracts()
{
    cout << updatedFrame->size() << endl;
    cout << extractRectOne.size() << endl;
    
    Mat tempExtractOne = Mat(*updatedFrame, extractRectOne),
    tempExtractTwo = Mat(*updatedFrame, extractRectTwo),
    tempExtractThree = Mat(*updatedFrame, extractRectThree),
    tempExtractFour = Mat(*updatedFrame, extractRectFour),
    tempExtractFive = Mat(*updatedFrame, extractRectFive),
    tempExtractSix = Mat(*updatedFrame, extractRectSix),
    tempExtractSeven = Mat(*updatedFrame, extractRectSeven),
    tempExtractEight = Mat(*updatedFrame, extractRectEight);
    
    extractFrameStorage.push_back(tempExtractOne);
    extractFrameStorage.push_back(tempExtractTwo);
    extractFrameStorage.push_back(tempExtractThree);
    extractFrameStorage.push_back(tempExtractFour);
    extractFrameStorage.push_back(tempExtractFive);
    extractFrameStorage.push_back(tempExtractSix);
    extractFrameStorage.push_back(tempExtractSeven);
    extractFrameStorage.push_back(tempExtractEight);
}

void BinarySkinFilter::importFrameWithContrast(Mat &frame, int contrastMultiplier)
{
    Mat contrastedFrame = Mat(frame.rows, frame.cols, CV_8UC3);
    imgMultiplier = contrastMultiplier;
    
    frame.convertTo(contrastedFrame, -1, contrastMultiplier, 0);
    updatedFrame = &frame;
}

void BinarySkinFilter::importFrameOriginal(Mat &frame)
{
    imgMultiplier = 1; //no difference to contrast
    
    updatedFrame = &frame;
}












