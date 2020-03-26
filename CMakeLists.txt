cmake_minimum_required(VERSION 3.0)
project(POBR)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED) 
set(PCL_BUILD_TYPE Release) 

include_directories(${OpenCV_INCLUDE_DIRS} include/)

aux_source_directory(src/POBR POBR_SRC_FILES)
add_library(POBR SHARED ${POBR_SRC_FILES})
target_link_libraries(POBR ${OpenCV_LIBS})

aux_source_directory(src/ DETECT_BK_SRC_FILES) 
add_executable(DetectBK ${DETECT_BK_SRC_FILES})
target_link_libraries(DetectBK POBR ${OpenCV_LIBS} yaml-cpp)

add_executable(Resizers src/examples/Resizers.cxx)
target_link_libraries(Resizers POBR ${OpenCV_LIBS})

add_executable(Filters src/examples/Filters.cxx)
target_link_libraries(Filters POBR ${OpenCV_LIBS})