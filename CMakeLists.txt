
### ~~~~~~~~~~~~ General  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

cmake_minimum_required(VERSION 3.10)

project( PDF_Reader )

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON) # "https://cmake.org/cmake/help/v3.0/variable/CMAKE_INCLUDE_CURRENT_DIR.html" ( QT RUNS PROPERLY WHEN THIS IS SET? )

#set(CMAKE_PREFIX_PATH "C:/Users/gregg/Documents/ImageMagick-Windows-master/ImageMagick-Windows-master/ImageMagick")
### ~~~~~~~~~~~~ Find Packages ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
#find_package(ImageMagick 7 COMPONENTS Magick++)
find_package(OpenCV 4.0 REQUIRED)
find_package( Tesseract 3.05 REQUIRED )


### ~~~~~~~~~~~~ General Include/Add ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#set(ImageMagick_INCLUDE_DIR "C:/Users/gregg/Documents/ImageMagick-Windows-master/ImageMagick-Windows-master/ImageMagick")
#set(ImageMagick_INCLUDE_DIR2 "C:/Users/gregg/Documents/ImageMagick-Windows-master/ImageMagick-Windows-master/ImageMagick/Magick++/lib/")

#set(ImageMagick_INCLUDE_DIR "C:/Users/gregg/Documents/ImageMagick-Windows-master/ImageMagick-Windows-master/ImageMagick/Magick++/lib")

# IMPORTANT - "include_directories" must come before "add_subdirectory"!
include_directories(include ${OpenCV_INCLUDE_DIR} ${Tesseract_INCLUDE_DIRS})# ${ImageMagick_INCLUDE_DIR} ${ImageMagick_INCLUDE_DIR2})

add_subdirectory(src)
add_subdirectory(tst)
add_subdirectory(clientApp)
add_subdirectory(lib/googletest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
