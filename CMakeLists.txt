cmake_minimum_required(VERSION 3.0)
project(DisplayImage)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(DisplayImage DisplayImage.cxx)
target_link_libraries(DisplayImage ${OpenCV_LIBS})

add_executable(ColourSpaceReduction ColourSpaceReduction.cxx)
target_link_libraries(ColourSpaceReduction ${OpenCV_LIBS})

