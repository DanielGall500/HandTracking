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
    
    //For Each Extract
    for(int ext = 0; ext < extractFrameStorage.size(); ext++)
    {
        //Most Dominant Colours
        cols = findDominantColours(extractFrameStorage[ext], dominantColours);
        
        //Add to global vector
        for (Vec3b filterColour : cols)
        {
            filterColours.push_back(filterColour);
        }
    }
    
    //Find Thresholds
    findMinMaxChannels(filterColours, filterThreshold, minB, maxB, minG, maxG, minR, maxR);
}

vector<Vec3b> BinarySkinFilter::findDominantColours(Mat extractFrame, int amountOfColours)
{
    /*These arrays are a counter for each individual 
      shade of b, g and r.
     
     If we increment any individual element of this array,
     this means that shade of has been found and thus is 
     more dominant in the extract image. */
     
    int blueCollection[255]  = {0},
        greenCollection[255] = {0},
        redCollection[255]   = {0};
    
    Vec3b channels;
    Vec3b filterPixel;
    
    vector<Vec3b> filterPixels;
    
    //For Each Pixel in Extract
    for (short int row = 0; row <= extractFrame.rows; row++)
    {
        for (short int col = 0; col <= extractFrame.cols; col++)
        {
            channels = extractFrame.at<Vec3b>(row, col);
            
            int b = channels[0],
                g = channels[1],
                r = channels[2];
            
            /*For each pixel, we note which colours are 
              being found most often */
            
            //POSSIBLY b - 1???
            blueCollection[b]++;
            greenCollection[g]++;
            redCollection[r]++;
        }
    }
    
    //Find N Colours (specified in func inp)
    for (int i = 0; i < amountOfColours; i++)
    {
        /* Find the mode of each array.
         
           The mode in this case represents the most
           commonly found shade of blue, green and red
           in the extract */
        
        int blueMode  = findChannelMax(blueCollection),
            greenMode = findChannelMax(greenCollection),
            redMode   = findChannelMax(redCollection);
        
        //Create a colour to use as a filter
        filterPixel = Vec3b(blueMode, greenMode, redMode);
        filterPixels.push_back(filterPixel);
        
        //We discount these shades for the next colour
        blueCollection[blueMode] = -1;
        greenCollection[greenMode] = -1;
        redCollection[redMode] = -1;
    }
    
    return filterPixels;
}

Mat BinarySkinFilter::runBinaryFiltering(Mat frame)
{
    //Create a matrix of placeholder pixels, size of frame
    Mat outputImage(frame.size(), CV_8UC3, Scalar(0,0,0));
    
    Vec3b *rowPixel;

    //For Each Row
    for (int row = 0; row <= frame.rows; row++)
    {
        rowPixel = frame.ptr<Vec3b>(row);
    
        //For Each Column
        for (int col = 0; col <= frame.cols; col++)
        {
            //Store the rgb colours of this pixel
            int b = rowPixel[col][0],
                g = rowPixel[col][1],
                r = rowPixel[col][2];
            
            /* If the pixel falls within the skin
               colour threshold, put a white pixel at
               that location on the output frame.
               Else, leave it as black. */
            
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


void BinarySkinFilter::showExtractAreas(Mat frame, Rect extracts[totalExtracts], Scalar colour)
{
    //Draws the extracts onto a frame
    
    for (int i = 0; i < totalExtracts; i++)
    {
        rectangle(frame, extracts[i], colour);
    }
}

void BinarySkinFilter::collectImageExtracts(Rect extracts[])
{
    extractFrameStorage.clear();
    
    for(int i = 0; i < totalExtracts; i++)
    {
        Mat tmpExt = Mat(*updatedFrame, extracts[i]);
        
        extractFrameStorage.push_back(tmpExt);
        
    }
}

void BinarySkinFilter::importFrameOriginal(Mat &frame)
{
    updatedFrame = &frame;
}












