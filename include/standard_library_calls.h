#pragma once
#ifndef STANDARD_LIB_CALLS_H
#define STANDARD_LIB_CALLS_H

#include <string>


class STANDARD_LIB_CALLS {
public:
    // Check video-capture opens and reads a frame which is not empty
    static int check_concurrency_method();
    static int check_images_present();
};

#endif 


