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

void BinarySkinFilter::runColourCollection(int filterThreshold)
{
    Vec3b col;
    
    for(int ext = 0; ext <= extractFrameStorage.size() - 1; ext++)
    {
        col = findDominantColour(extractFrameStorage[ext]);
        
        filterColours.push_back(col);
    }
    
    findMinMaxChannels(filterColours, filterThreshold, minB, maxB, minG, maxG, minR, maxR);
}

/*void BinarySkinFilter::runBinaryFiltering(Mat &frame, int threshold)
{
    Mat img;
    binaryImages.clear();
    
    for (int filt = 0; filt <= filterColours.size() - 1; filt++)
    {
        img = filterImage();
        
        binaryImages.push_back(img);
    }
}*/

void BinarySkinFilter::collectImageExtracts()
{
    Mat tempExtractOne = Mat(*originalFrame, extractRectOne),
        tempExtractTwo = Mat(*originalFrame, extractRectTwo),
        tempExtractThree = Mat(*originalFrame, extractRectThree),
        tempExtractFour = Mat(*originalFrame, extractRectFour),
        tempExtractFive = Mat(*originalFrame, extractRectFive),
        tempExtractSix = Mat(*originalFrame, extractRectSix),
        tempExtractSeven = Mat(*originalFrame, extractRectSeven);
    
    extractFrameStorage.push_back(tempExtractOne);
    extractFrameStorage.push_back(tempExtractTwo);
    extractFrameStorage.push_back(tempExtractThree);
    extractFrameStorage.push_back(tempExtractFour);
    extractFrameStorage.push_back(tempExtractFive);
    extractFrameStorage.push_back(tempExtractSix);
    extractFrameStorage.push_back(tempExtractSeven);
}

Vec3b BinarySkinFilter::findDominantColour(Mat extractFrame)
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
    
    Vec3b filterPixel = Vec3b(blueMode, greenMode, redMode);
    
    return filterPixel;
}

Mat BinarySkinFilter::runBinaryFiltering()
{
    resize(*originalFrame, *originalFrame, Size(320,240), INTER_NEAREST);
    
    /*TODO
     resizing fixed, works great
     although implement it in the parameters instead and change
     from original frame, as it could mess up the other functions
     okay so need to extract info and apply it to bigger image now
     */
    
    Mat outputImage(originalFrame->size(), CV_8UC3, Scalar(0,0,0));
    Vec3b *rowPixel;

    for (int row = 0; row <= originalFrame->rows; row++)
    {
        rowPixel = originalFrame->ptr<Vec3b>(row);
    
        for (int col = 0; col <= originalFrame->cols; col++)
        {
            int b = rowPixel[col][0],
                g = rowPixel[col][1],
                r = rowPixel[col][2];
            
            if (withinFilterRange(b, g, r))
            {
                outputImage.at<Vec3b>(row, col) = Vec3b(255,255,255);
            }
        }
    }
    
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

bool BinarySkinFilter::withinFilterRange(int b, int g, int r)
{
    bool bPassed = false, gPassed = false, rPassed = false;
    int f = 0;
    
    auto channelInsideRange = [](int channel, int min, int max)
    { return ((unsigned)(channel - min) <= (max - min)); };
    
    while ((!bPassed && !gPassed && !rPassed) && f <= filterColours.size())
    {
        bPassed = channelInsideRange(b, minB[f], maxB[f]);
        gPassed = channelInsideRange(g, minG[f], maxB[f]);
        rPassed = channelInsideRange(r, minR[f], maxR[f]);
        
        f++;
    }
    return (bPassed && gPassed && rPassed);
}

void BinarySkinFilter::setExtractLocations(Rect extractRects[7])
{
    extractRectOne = extractRects[0]; //need to be kept seperate
    extractRectTwo = extractRects[1];
    extractRectThree = extractRects[2];
    extractRectFour = extractRects[3];
    extractRectFive = extractRects[4];
    extractRectSix = extractRects[5];
    extractRectSeven = extractRects[6];
}


void BinarySkinFilter::showExtractAreas(Mat frame, Rect extracts[7], Scalar colour)
{
    for (int i = 0; i <= totalExtracts; i++)
    {
        rectangle(frame, extracts[i], colour);
    }
}

/*
Mat BinarySkinFilter::getSummedBinaryImages()
{
    Mat summedImage = Mat(binaryImages[0].rows, binaryImages[0].cols, binaryImages[0].type(), double(0));
    
    for (int img = 0; img <= binaryImages.size() - 1; img++)
    {        
        add(summedImage, binaryImages[img], summedImage);
    }
    
    medianBlur(summedImage, summedImage, 7);
    
    return summedImage;
}*/














