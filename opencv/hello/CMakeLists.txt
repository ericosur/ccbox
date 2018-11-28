cmake_minimum_required(VERSION 3.0)

set(PN "hello")
project(${PN})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

#set(OpenCV_DIR    "/opt/intel/opencv/share/OpenCV")
set(OpenCV_DIR    "/usr/local/share/OpenCV")
find_package( OpenCV REQUIRED )
message(STATUS "    version: ${OpenCV_VERSION}")


add_executable(
    ${PN}
    main.cpp
)

target_link_libraries(
    ${PN}
    ${OpenCV_LIBS}
)
