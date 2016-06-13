//
//  main.cpp
//  HandTracking
//
//  Created by Daniel Gallagher on 13/06/2016.
//  Copyright (c) 2016 Developer Co. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, const char * argv[]) {
    VideoCapture cap(0);
    
    if(!cap.isOpened())
        return -1;
    
    while(true)
    {
        Mat frame, edges;
        
        cap >> frame;
        
        cvtColor(frame, frame, CV_BGR2GRAY);
        GaussianBlur(frame, frame, Size(7,7), 1.5);
        Canny(frame, edges, 0, 30);
        
        imshow("Edges", edges);
        
        if(waitKey(30) >= 0)
            break;
    }
    
    return 0;
}
