set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(Client_Project ${CMAKE_PROJECT_NAME}_ClientApp)

set(project_sources client.cpp)

set(project_headers client.h) 

add_executable(${Client_Project} ${project_sources} ${project_headers})

set( DEPENDENCIES ${OpenCV_LIBS} wsock32 ws2_32 Server) 

target_link_libraries(${Client_Project} PUBLIC ${DEPENDENCIES}) # target linked library of source_binary


