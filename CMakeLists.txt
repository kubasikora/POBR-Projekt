cmake_minimum_required(VERSION 3.0)
project(POBR)
find_package(OpenCV REQUIRED)

find_package(yaml-cpp REQUIRED) 
set(PCL_BUILD_TYPE Release) 

include_directories(${OpenCV_INCLUDE_DIRS} include/)
aux_source_directory(src/ POBR_SRC_FILES) 

add_executable(MaskImage ${POBR_SRC_FILES})
target_link_libraries(MaskImage ${OpenCV_LIBS} yaml-cpp)
