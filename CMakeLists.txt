cmake_minimum_required(VERSION 3.0)
project(DisplayImage)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} include/)
aux_source_directory(src/ POBR_SRC_FILES) 

add_executable(DisplayImage ${POBR_SRC_FILES})
target_link_libraries(DisplayImage ${OpenCV_LIBS})