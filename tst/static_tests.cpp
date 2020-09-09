#include "gtest/gtest.h"
#include "standard_library_calls.h"

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
/*
TEST(OPENCV_API_FUNCTION_CALLS, checkObjectDetectorInitialisation) {
    //arrange
    //act
    //assert
    ASSERT_EQ(OPENCV_API_CALLS::check_ObjectDetectorInitialisation(), true) << "Failed to initialise object detector";

}

TEST(OPENCV_API_FUNCTION_CALLS, detectAndDrawLines) {
    //arrange
    //act
    //assert
    ASSERT_EQ(OPENCV_API_CALLS::detectAndDrawLines(), true) << "Failed to detect lines in the image";

}
*/

