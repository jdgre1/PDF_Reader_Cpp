

### ~~~~~~~~~~~~ BUILD RELEVANT LIBRARIES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

add_library(PageProcessor STATIC page_processor.cpp ../include/page_processor.h)
target_link_libraries(PageProcessor ${OpenCV_LIBS} ${Tesseract_LIBRARIES})

add_library(OPENCV_API_CALLS STATIC opencv_api_calls.cpp ../include/opencv_api_calls.h)
add_library(TESSERACT_API_CALLS STATIC tesseract_api_calls.cpp ../include/tesseract_api_calls.h)
add_library(STANDARD_LIB_CALLS STATIC standard_library_calls.cpp ../include/standard_library_calls.h)
add_library(Server STATIC server.cpp ../include/server.h)
target_link_libraries(Server ${OpenCV_LIBS} wsock32 ws2_32)
target_link_libraries(OPENCV_API_CALLS ${OpenCV_LIBS})
target_link_libraries(TESSERACT_API_CALLS ${Tesseract_LIBRARIES})


### ~~~~~~~~~~~~ QT SETTINGS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### ~~~~~~~~~~~~ CREATE EXECUTABLE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

set(project_sources main.cpp PDFReaderRun.cpp)
add_executable(PDF_Reader ${project_sources} )
target_link_libraries(PDF_Reader PageProcessor TESSERACT_API_CALLS OPENCV_API_CALLS STANDARD_LIB_CALLS wsock32 ws2_32) # ${ImageMagick_LIBRARIES})
target_precompile_headers(PDF_Reader PRIVATE ../include/pch.h)