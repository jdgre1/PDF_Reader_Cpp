#pragma once
#ifndef TESSERACT_API_CALLS_H
#define TESSERACT_API_CALLS_H

#include <string>
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

class TESSERACT_API_CALLS {
public:
    // Check video-capture opens and reads a frame which is not empty
    static bool checkTesseractObjectInitialisation();



};
#endif 


