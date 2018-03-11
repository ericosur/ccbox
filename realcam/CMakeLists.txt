cmake_minimum_required(VERSION 3.0)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_C_FLAGS "-Wall -O3")

# Define project name
set(PN "realcam")
project(${PN})

find_package( OpenCV REQUIRED )

# realsense2
message("apply intel realsense2")
#include_directories()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_REALSENSE")
set(DEPENDENCIES realsense2 )

# pbox library
include_directories("/usr/local/include/pbox")
link_directories("/usr/local/lib/pbox")

add_executable(${PN}
    rs_camera_test.cpp
)

target_link_libraries(
    ${PN}
    libpbox.so
    ${DEPENDENCIES}
    ${OpenCV_LIBS}
)
