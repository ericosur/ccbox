cmake_minimum_required(VERSION 3.0)

set(PN "tracker")
project(${PN})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

find_package( OpenCV REQUIRED )

#include_directories("/home/rasmus/src/github/opencv/include")
#include_directories("/home/rasmus/src/github/opencv/modules/video/include")
#link_directories("/home/rasmus/src/github/opencv/build/lib")

add_executable(
    ${PN}
    main.cpp
)

target_link_libraries(
    ${PN}
    ${OpenCV_LIBS}
)
