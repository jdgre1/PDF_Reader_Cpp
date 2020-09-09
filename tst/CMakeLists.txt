set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(TST_BINARY ${CMAKE_PROJECT_NAME}_Testing)

set(project_sources parameterised_tests.cpp static_tests.cpp fixture_tests.cpp main.cpp)

set(project_headers static_tests.h parameterised_tests.h fixture_tests.h) 

add_executable(${TST_BINARY} ${project_sources} ${project_headers})

add_test(NAME ${TST_BINARY} COMMAND ${TST_BINARY})

set( DEPENDENCIES ${OpenCV_LIBS} ${Tesseract_LIBRARIES} gtest OPENCV_API_CALLS STANDARD_LIB_CALLS PageProcessor )

target_link_libraries(${TST_BINARY} PUBLIC ${DEPENDENCIES}) # target linked library of source_binary

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
