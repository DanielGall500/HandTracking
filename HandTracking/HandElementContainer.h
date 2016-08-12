//
//  HandElementContainer.h
//  HandTracking
//
//  Created by Daniel Gallagher on 11/08/2016.
//  Copyright (c) 2016 Developer Co. All rights reserved.
//

#ifndef __HandTracking__HandElementContainer__
#define __HandTracking__HandElementContainer__

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <stdio.h>

using namespace cv;
using namespace std;

typedef vector<vector<Point>> VVPoints;
typedef vector<vector<Vec4i>> VVVec4i;
typedef vector<vector<int>> VVInts;

class HandElementContainer
{
public:
    HandElementContainer();
    
    ~HandElementContainer();
    
    void findHandProperties(Mat binaryImage, VVPoints &outputContours,
                            VVPoints &outputHull, VVInts &hullInts,
                            VVVec4i &outputDefects, int topNContours = 1);
    
    void dismissIrrelevantDefects(VVVec4i &defects, VVPoints contours, float angleThresh);

    Mat drawDefects(Mat frame, VVVec4i defects, VVPoints contours);
    
private:
    
    VVPoints getNMaxContours(VVPoints contours, int n);
    
};



#endif /* defined(__HandTracking__HandElementContainer__) */
