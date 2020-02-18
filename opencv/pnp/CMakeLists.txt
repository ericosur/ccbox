cmake_minimum_required(VERSION 3.0)

set(PN "pnp")
project(${PN})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

# RECOMMEND:
# source /opt/intel/openvino/bin/setupvars.sh
message("OpenCV_DIR: ${OpenCV_DIR}")
find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Eigen3)
message("eigen3: ${EIGEN3_INCLUDE_DIR}")
include_directories(${EIGEN3_INCLUDE_DIR})

message(STATUS "opencv version: ${OpenCV_VERSION}")

add_executable(
    ${PN}
    main.cpp
    marker.cpp
    marker.h
)

target_link_libraries(
    ${PN}
    ${OpenCV_LIBS}
)
