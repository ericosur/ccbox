cmake_minimum_required(VERSION 3.0)

set(PN "tracker")
project(${PN})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

# intel opencv does not include tracking modules

# RECOMMEND:
# source /opt/intel/openvino/bin/setupvars.sh
# opencv2, opencv3
#set(OpenCV_DIR    "/usr/local/share/OpenCV")
# opencv4
set(OpenCV_DIR    "/usr/local/share/opencv4")
find_package( OpenCV REQUIRED )

include_directories("/usr/local/include")
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(
    ${PN}
    main.cpp
)

target_link_libraries(
    ${PN}
    ${OpenCV_LIBS}
)
