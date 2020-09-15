#include "gtest/gtest.h"
#include "standard_library_calls.h"
#include <tesseract_api_calls.h>

using namespace std;



TEST(STANDARD_LIB_CALLS, check_concurrency_method) {
    //arrange
    //act
    //assert

    ASSERT_GE(STANDARD_LIB_CALLS::check_concurrency_method(), 2) << "At least two processor cores were not detected";

}

TEST(STANDARD_LIB_CALLS, check_images_present) {
    //arrange
    //act
    //assert

    ASSERT_GE(STANDARD_LIB_CALLS::check_images_present(), 1) << "Folder does not contain any images";

}

TEST(TESSERACT_API_CALLS, checkTesseractObjectInitialisation) {
    //arrange
    //act
    //assert

    ASSERT_TRUE(TESSERACT_API_CALLS::checkTesseractObjectInitialisation()) << "Tesseract could not be initialised with English language library";

}


