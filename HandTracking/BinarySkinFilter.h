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
    
    //Import frame
    void importFrameOriginal(Mat &frame);
    
    //Show extract areas on video feed
    void showExtractAreas(Mat frame, Rect extracts[totalExtracts], Scalar colour);
    
    //Call when the extracts should be taken for processing
    void collectImageExtracts(Rect extracts[]);
    
    //Find filter colours after extracts have been collected
    void runColourCollection(int filterThreshold, int dominantColours);
    
    //Filter frame based on filter colours
    Mat runBinaryFiltering(Mat frame);
    
    std::vector<Vec3b> getFilterCols() { return filterColours; };
    
private:
    Mat *updatedFrame;
    
    vector<Mat> extractFrameStorage;
    vector<Vec3b> filterColours;
    
    //Find the most commonly occurring colours in an image
    vector<Vec3b> findDominantColours(Mat extractFrame, int amountOfColours);
    
    /* Thresholds:
       These are the minimum/maximum vals that each channel
       can be to pass the filter test */
    std::vector<int> minB, maxB, minG, maxG, minR, maxR;
    
    void findMinMaxChannels(std::vector<Vec3b> filter, int thresh,
                            std::vector<int> &minB, std::vector<int> &maxB,
                            std::vector<int> &minG, std::vector<int> &maxG,
                            std::vector<int> &minR, std::vector<int> &maxR);
    
    int findChannelMax(int colours[]);
    bool withinFilterRange(int b, int g, int r);
    
};


#endif /* defined(__HandTracking__BinarySkinFilter__) */
