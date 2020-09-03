

### ~~~~~~~~~~~~ BUILD RELEVANT LIBRARIES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

add_library(PageProcessor STATIC page_processor.cpp ../include/page_processor.h)
target_link_libraries(PageProcessor ${OpenCV_LIBS} ${Tesseract_LIBRARIES})

add_library(OPENCV_API_CALLS STATIC opencv_api_calls.cpp ../include/opencv_api_calls.h)
target_link_libraries(OPENCV_API_CALLS ${OpenCV_LIBS})

### ~~~~~~~~~~~~ QT SETTINGS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### ~~~~~~~~~~~~ CREATE EXECUTABLE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

set(project_sources main.cpp PDFReaderRun.cpp)
add_executable(PDF_Reader ${project_sources} )
target_link_libraries(PDF_Reader PageProcessor OPENCV_API_CALLS)
target_precompile_headers(PDF_Reader PRIVATE ../include/pch.h)