
### ~~~~~~~~~~~~ General  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

cmake_minimum_required(VERSION 3.10)

project( PDF_Reader )

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON) # "https://cmake.org/cmake/help/v3.0/variable/CMAKE_INCLUDE_CURRENT_DIR.html" ( QT RUNS PROPERLY WHEN THIS IS SET? )


### ~~~~~~~~~~~~ Find Packages ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(OpenCV 4.0 REQUIRED)
find_package( Tesseract 3.05 REQUIRED )


### ~~~~~~~~~~~~ General Include/Add ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

include_directories(include ${OpenCV_INCLUDE_DIR} ${Tesseract_INCLUDE_DIRS})
add_subdirectory(src)
add_subdirectory(tst)
add_subdirectory(lib/googletest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)