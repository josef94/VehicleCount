cmake_minimum_required(VERSION 2.8) 
PROJECT ( VehicleCount )
find_package(OpenCV REQUIRED )
set( NAME_SRC
    ../VehicleCount.cpp
    ../Blob.cpp
    ../FileHandler.cpp
)
set( NAME_HEADERS       
     ../Blob.h
     ../FileHandler.h
) 
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )
link_directories( ${CMAKE_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable( VehicleCount ${NAME_SRC} ${NAME_HEADERS} )
target_link_libraries(VehicleCount ${OpenCV_LIBS} )
