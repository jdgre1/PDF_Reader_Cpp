#pragma once
#ifndef OPENCV_API_PARAMTEST__H__ 
#define OPENCV_API_PARAMTEST__H__
#include <static_tests.h>
#include "gtest/gtest.h"


class OpenCV_API_ParamTest :public ::testing::TestWithParam<int> {

public:
    int trackerIndex;
    
    bool tempVar;
};

#endif 


