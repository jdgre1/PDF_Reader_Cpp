#include "gtest/gtest.h"
#include <standard_library_calls.h>
//#include <boost/locale.hpp>

#define DEBUG_BUILD 1;

int main(int argc, char** argv) {


#if DEBUG_BUILD

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#else
#endif


}