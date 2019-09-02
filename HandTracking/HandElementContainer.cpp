//
//  HandElementContainer.cpp
//  HandTracking
//
//  Created by Daniel Gallagher on 11/08/2016.
//  Copyright (c) 2016 Developer Co. All rights reserved.
//

#include "HandElementContainer.h"

HandElementContainer::HandElementContainer()
{
    
}

HandElementContainer::~HandElementContainer()
{
    
}

//FINDING THE SHAPE OF THE HAND
void HandElementContainer::findHandProperties(Mat binaryImage, VVPoints &outputContours,
                                              VVPoints &outputHull, VVInts &outputHullInts,
                                              VVVec4i &outputDefects, int topNContours)
{
    Mat editedImg;
    
    //Convert to black/white
    cvtColor(binaryImage, editedImg, CV_BGR2GRAY);
    
    VVPoints contours;
    VVPoints maxContours;
    vector<Vec4i> hierarchy;
   
    //Store the contour points
    findContours(editedImg, contours, hierarchy, CV_RETR_TREE,
                 CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    
    if (contours.size() > 0)
    {
    
        maxContours = getNMaxContours(contours, topNContours);
        
        //Form the shape of the hand
        VVPoints hull(maxContours.size());
        VVInts hullInts(maxContours.size());
        VVVec4i defects(maxContours.size());
        
        for (int i = 0; i < maxContours.size(); i++)
        {
            convexHull(maxContours[i], hull[i], false);
            convexHull(maxContours[i], hullInts[i], false);
            
            if (hullInts[i].size() > 3)
            {
                convexityDefects(maxContours[i], hullInts[i], defects[i]);
            }
        }
        
        outputContours = maxContours;
        outputHullInts = hullInts;
        outputDefects  = defects;
        outputHull     = hull;
    }
}

VVPoints HandElementContainer::getNMaxContours(VVPoints contours, int n)
{
    VVPoints nContours;
    vector<double> contourAreas;
    vector<int> foundMax;
    
    for (auto contour : contours)
    {
        double area = contourArea(contour);
        contourAreas.push_back(area);
    }
    
    
    for (int i = 0; i <= n; i++)
    {
        int maxPosition = 0;
        
        //For Each Contour
        for (int j = 0; j < contourAreas.size(); j++)
        {
            //Find Max
            if (contourAreas[j] > contourAreas[maxPosition] &&
                std::find(foundMax.begin(), foundMax.end(), maxPosition) == foundMax.end())
            {
                //Update Max Position
                maxPosition = j;
            }
        }
        
        nContours.push_back(contours[maxPosition]);
        
        foundMax.push_back(maxPosition);
    }
    
    return nContours;
}

Mat HandElementContainer::drawDefects(Mat frame, VVVec4i defects, VVPoints contours)
{
    Mat handConvexityDefects = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
    
    if (defects.size() >= 1 && contours.size() >= 1)
    {
        for (int defIdx = 0; defIdx < contours.size(); defIdx++)
        {
            for (int i = 0; i < defects[defIdx].size(); i++)
            {
                Vec4i& v = defects[defIdx][i];
                
                float depth = v[3] / 256;
                if (depth > 10) //mess around with this
                {
                    int startIdx = v[0];
                    Point ptStart(contours[defIdx][startIdx]);
                    
                    int farIdx = v[1];
                    Point ptFar(contours[defIdx][farIdx]);
                    
                    int endIdx = v[2];
                    Point ptEnd(contours[defIdx][endIdx]);
                    
                    Scalar defectCol(0,0,255);
                    
                    line(handConvexityDefects, ptStart, ptEnd, Scalar(255,0,0), 1); //blue
                    line(handConvexityDefects, ptStart, ptFar, Scalar(0,255,0), 1); //green
                    line(handConvexityDefects, ptEnd, ptFar, Scalar(0,0,255), 1); //red
                    circle(handConvexityDefects, ptFar, 4, Scalar(255,255,255), 2); //white

                }
            }
        }
    }
    
    return handConvexityDefects;
}

void HandElementContainer::dismissIrrelevantInfo(VVVec4i &defects, VVPoints contours, float angleThresh,
                                                    vector<int> &fingerAngles)
{
    VVVec4i passedDefects, failedDefects;
    
    for (int idx = 0; idx < contours.size(); idx++)
    {
        vector<Vec4i> subDefects, throwAways;
        
        for (int i = 0; i < defects[idx].size(); i++)
        {
            Vec4i defect = defects[idx][i];
            
            int startIdx = defect[0], endIdx = defect[1], farIdx = defect[2];
            
            Point ptStart(contours[idx][startIdx]), ptFar(contours[idx][farIdx]),
            ptEnd(contours[idx][endIdx]);
            
            float startEndSlp = findSlope(ptStart, ptEnd), //blue slope 1
            startFarSlp = findSlope(ptStart, ptFar), //green
            endFarSlp = findSlope(ptEnd, ptFar); //red slope 2
            
            //find angle between two slopes
            float commonAngle = (float(startFarSlp - endFarSlp) /
                                 float(1 - (startFarSlp * endFarSlp))) * 100;
            
            if (commonAngle <= angleThresh && commonAngle >= -angleThresh)
            {
                subDefects.push_back(defect);
            }
            else
            {
                throwAways.push_back(defect);
            }
            
            fingerAngles.push_back(commonAngle);
        }
        passedDefects.push_back(subDefects);
        failedDefects.push_back(throwAways);
    }
    
    pFailedDefects = &failedDefects;
    defects = passedDefects;
}

float HandElementContainer::findSlope(Point pt1, Point pt2)
{
    float slope;
    
    float yDist = pt2.y - pt1.y;
    float xDist = pt2.x - pt1.x;
    
    // make sure we don't divide by zero
    // for the slope as makes hardware implode
    
    if (xDist == 0 || yDist == 0)
        slope = 0;
    else
        slope = yDist / xDist;
    
    return slope;
}









