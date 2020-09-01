

### ~~~~~~~~~~~~ BUILD RELEVANT LIBRARIES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### ~~~~~~~~~~~~ QT SETTINGS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### ~~~~~~~~~~~~ CREATE EXECUTABLE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

set(project_sources main.cpp)
add_executable(PDF_Reader ${project_sources} )
target_link_libraries(PDF_Reader ${OpenCV_LIBS} ${Tesseract_LIBRARIES})