cmake_minimum_required(VERSION 3.0)

set(PN "hello")
project(${PN})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")


# RECOMMEND:
# source /opt/intel/openvino/bin/setupvars.sh
find_package( OpenCV REQUIRED )
message(STATUS "    version: ${OpenCV_VERSION}")

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(
    ${PN}
    main.cpp
)

target_link_libraries(
    ${PN}
    ${OpenCV_LIBS}
)
