#pragma once
#ifndef OPENCV_API_CALLS_H
#define OPENCV_API_CALLS_H

#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/trace.hpp>
#include <opencv2/tracking/tracker.hpp>

class OPENCV_API_CALLS {
public:
    // Check video-capture opens and reads a frame which is not empty
    static bool check_VideoCaptureWorks();
    
    // Check that an object tracker can be initialised successfully
    static bool check_ObjectTrackerInitialisation(int tracker_index);

    // Check that an object detector can be initialised successfully
    static bool check_ObjectDetectorInitialisation();

    static bool detectAndDrawLines();
};

#endif 


