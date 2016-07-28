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
    BinarySkinFilter();
    
    ~BinarySkinFilter();
    
    void updateFrame(cv::Mat &frame) { originalFrame = &frame; }
    
    void showExtractAreas(cv::Mat &frame, cv::Rect extracts[7]); //YOU SHOULD GIVE EXTRACT OPTIONS TO MAIN
    
    void runExtractCollection(cv::Rect extractRects[7]) { setExtractLocations(extractRects); collectImageExtracts(); };
    
    void runColourCollection();
    
    void runBinaryFiltering(cv::Mat &frame, int threshold);
    
    cv::Mat getSummedBinaryImages();
    
    std::vector<cv::Mat> getBinaryImages() { return binaryImages; }
    
private:
    cv::Mat *originalFrame;
    
    int totalExtracts = 7;
    
    std::vector<cv::Mat> extractFrameStorage;
    
    cv::Rect extractRectOne, extractRectTwo, extractRectThree, extractRectFour,
    extractRectFive, extractRectSix, extractRectSeven;
    
    std::vector<cv::Vec3b> filterColours;
    
    cv::Vec3b findDominantColour(cv::Mat extractFrame);
    
    cv::Mat filterImage(cv::Vec3b &filterChannels, int threshold);
    
    std::vector<cv::Mat> binaryImages;
    
    void setExtractLocations(cv::Rect extractRects[7]);
    void collectImageExtracts();
    
    int findChannelMax(int colours[]);
    bool withinRange(int val, int min, int max);
    
};







#endif /* defined(__HandTracking__BinarySkinFilter__) */
