//
//  BinarySkinFilter.h
//  HandTracking
//
//  Created by Daniel Gallagher on 24/07/2016.
//  Copyright (c) 2016 Developer Co. All rights reserved.
//

#ifndef __HandTracking__BinarySkinFilter__
#define __HandTracking__BinarySkinFilter__

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <stdio.h>
#include <vector>
#include <iostream>
#include <array>

const int totalExtracts = 8;

using namespace std;
using namespace cv;

class BinarySkinFilter
{
public:
    BinarySkinFilter();
    
    ~BinarySkinFilter();
    
    void importFrameWithContrast(Mat &frame, int contrastMultiplier);
    
    void importFrameOriginal(Mat &frame);
    
    void showExtractAreas(Mat frame, Rect extracts[totalExtracts], Scalar colour);
    
    void runExtractCollection(Rect extractRects[totalExtracts]) { setExtractLocations(extractRects); collectImageExtracts(); };
    
    void runColourCollection(int filterThreshold, int dominantColours);
    
    Mat runBinaryFiltering(Mat frame);
    
private:
    Mat *updatedFrame;
    int imgMultiplier;
    
    std::vector<Mat> extractFrameStorage;
    
    Rect extractRectOne, extractRectTwo, extractRectThree, extractRectFour,
    extractRectFive, extractRectSix, extractRectSeven, extractRectEight;
    
    std::vector<Vec3b> filterColours;
    
    vector<Vec3b> findDominantColours(Mat extractFrame, int multiplier, int amountOfColours);
    
    void setExtractLocations(Rect extractRects[totalExtracts]);
    void collectImageExtracts();
    
    void increaseContrast(Mat &frame, int difference); //makes colours more distinct
    
    std::vector<int> minB, maxB, minG, maxG, minR, maxR;
    
    void findMinMaxChannels(std::vector<Vec3b> filter, int thresh,
                            std::vector<int> &minB, std::vector<int> &maxB,
                            std::vector<int> &minG, std::vector<int> &maxG,
                            std::vector<int> &minR, std::vector<int> &maxR);
    
    int findChannelMax(int colours[]);
    bool withinFilterRange(int b, int g, int r);
    
};


#endif /* defined(__HandTracking__BinarySkinFilter__) */
