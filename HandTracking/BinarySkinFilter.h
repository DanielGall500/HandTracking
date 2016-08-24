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

class BinarySkinFilter
{
public:
    BinarySkinFilter(int imgMultiplier = 1);
    
    ~BinarySkinFilter();
    
    void importFrameWithContrast(cv::Mat &frame);
    
    void showExtractAreas(cv::Mat frame, cv::Rect extracts[7], cv::Scalar colour);
    
    void runExtractCollection(cv::Rect extractRects[7]) { setExtractLocations(extractRects); collectImageExtracts(); };
    
    void runColourCollection(int filterThreshold);
    
    cv::Mat runBinaryFiltering(cv::Mat frame);
    
private:
    cv::Mat *updatedFrame;
    int contrastMultiplier;
    
    int totalExtracts = 7;
    
    std::vector<cv::Mat> extractFrameStorage;
    
    cv::Rect extractRectOne, extractRectTwo, extractRectThree, extractRectFour,
    extractRectFive, extractRectSix, extractRectSeven;
    
    std::vector<cv::Vec3b> filterColours;
    
    cv::Vec3b findDominantColour(cv::Mat extractFrame, int multiplier);
    
    void setExtractLocations(cv::Rect extractRects[7]);
    void collectImageExtracts();
    
    void increaseContrast(cv::Mat &frame, int difference); //makes colours more distinct
    
    std::vector<int> minB, maxB, minG, maxG, minR, maxR;
    
    void findMinMaxChannels(std::vector<cv::Vec3b> filter, int thresh,
                            std::vector<int> &minB, std::vector<int> &maxB,
                            std::vector<int> &minG, std::vector<int> &maxG,
                            std::vector<int> &minR, std::vector<int> &maxR);
    
    int findChannelMax(int colours[]);
    bool withinFilterRange(int b, int g, int r);
    
};


#endif /* defined(__HandTracking__BinarySkinFilter__) */
