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

void BinarySkinFilter::runColourCollection()
{
    Vec3b col;
    
    cout << "Finding Dominant Colours" << endl;
    
    for(int ext = 0; ext <= extractFrameStorage.size() - 1; ext++)
    {
        cout << "Extract Num " << ext << endl;
        
        col = findDominantColour(extractFrameStorage[ext]);
        
        cout << "Colour: " << col << endl;
        
        filterColours.push_back(col);
    }
    cout << "Finished Dominant Colours" << endl;
}

void BinarySkinFilter::runBinaryFiltering(Mat &frame, int threshold)
{
    Mat img;
    binaryImages.clear();
    
    cout << "Inside Binary Filter" << endl;
    
    for (int filt = 0; filt <= filterColours.size() - 1; filt++)
    {
        cout << "Filter Number " << filt << endl;
        img = filterImage(filterColours[filt], threshold);
        
        binaryImages.push_back(img);
    }
}

void BinarySkinFilter::collectImageExtracts()
{
    Mat tempExtractOne = Mat(*originalFrame, extractRectOne),
        tempExtractTwo = Mat(*originalFrame, extractRectTwo),
        tempExtractThree = Mat(*originalFrame, extractRectThree),
        tempExtractFour = Mat(*originalFrame, extractRectFour);
    
    cout << "TEMPEXTRACT" << endl;
    cout << tempExtractOne.at<Vec3b>(0,0) << endl;
    
    extractFrameStorage.push_back(tempExtractOne);
    extractFrameStorage.push_back(tempExtractTwo);
    extractFrameStorage.push_back(tempExtractThree);
    extractFrameStorage.push_back(tempExtractFour);
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
            
            cout << "Running Colour Search: R - " << row << " C - " << col << endl;
        }
    }
    
    int blueMode = findChannelMax(blueCollection),
    greenMode = findChannelMax(greenCollection),
    redMode = findChannelMax(redCollection);
    
    Vec3b filterPixel = Vec3b(blueMode, greenMode, redMode);
    
    return filterPixel;
}

Mat BinarySkinFilter::filterImage(Vec3b &filterChannels, int threshold)
{
    Mat outputImage(originalFrame->size(), CV_8UC3, Scalar(0,0,0));
    
    for (int row = 0; row <= originalFrame->rows; row++) //probably a better method for this
    {
        for (int col = 0; col <= originalFrame->cols; col++)
        {
            Vec3b originalPixel = originalFrame->at<Vec3b>(row, col);
            
            int b = originalPixel[0],
            g = originalPixel[1],
            r = originalPixel[2];
            
            int minBlueIntensity = filterChannels[0] - threshold,
            maxBlueIntensity = filterChannels[0] + threshold;
            
            int minGreenIntensity = filterChannels[1] - threshold,
            maxGreenIntensity = filterChannels[1] + threshold;
            
            int minRedIntensity = filterChannels[2] - threshold,
            maxRedIntensity = filterChannels[2] + threshold;
            
            if (withinRange(b, minBlueIntensity, maxBlueIntensity) &&
                withinRange(g, minGreenIntensity, maxGreenIntensity) &&
                withinRange(r, minRedIntensity, maxRedIntensity))
                outputImage.at<Vec3b>(row, col) = Vec3b(255,255,255);
        }
    }
    cout << "We got to the end of the fitler method" << endl;
    
    return outputImage;
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

bool BinarySkinFilter::withinRange(int val, int min, int max)
{
    return (unsigned)(val - min) <= (max - min);
}

void BinarySkinFilter::setExtractLocations(Rect extractRects[4])
{
    extractRectOne = extractRects[0]; //need to be kept seperate
    extractRectTwo = extractRects[1];
    extractRectThree = extractRects[2];
    extractRectFour = extractRects[3];
}


void BinarySkinFilter::showExtractAreas(Mat &frame, Rect extracts[4])
{
    rectangle(frame, extracts[0],
              Scalar(0,255,0));
    
    rectangle(frame, extracts[1],
              Scalar(0,255,0));
    
    rectangle(frame, extracts[2],
              Scalar(0,255,0));
    
    rectangle(frame, extracts[3],
              Scalar(0,255,0));
}

Mat BinarySkinFilter::getSummedBinaryImages()
{
    Mat summedImage = Mat(binaryImages[0].rows, binaryImages[0].cols, binaryImages[0].type(), double(0));
    
    for (int img = 0; img <= binaryImages.size() - 1; img++)
    {        
        add(summedImage, binaryImages[img], summedImage);
    }
    
    return summedImage;
}














