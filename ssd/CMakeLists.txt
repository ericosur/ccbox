cmake_minimum_required(VERSION 3.0)

set(PN  "ssd_detect")

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_C_FLAGS "-Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function -Wno-sign-compare")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPU_ONLY=1 -DUSE_OPENCV")

set(CAFFE_ROOT  "$ENV{HOME}/src/github/intel/caffe")
message("CAFFE_ROOT: ${CAFFE_ROOT}")

# pbox library
include_directories("/usr/local/include/pbox")
link_directories("/usr/local/lib/pbox")

# caffe library
include_directories("${CAFFE_ROOT}/build/install/include")
message("${CAFFE_ROOT}/build/install/include")

link_directories("${CAFFE_ROOT}/build/install/lib")
message("${CAFFE_ROOT}/build/install/lib")

link_directories(${CAFFE_ROOT}/external//mkldnn/install/lib)
link_directories(${CAFFE_ROOT}/external/mkl/mklml_lnx_2018.0.1.20171227/lib)
message(${CAFFE_ROOT}/external//mkldnn/install/lib)

set(DEPENDENCIES libboost_system.so gflags glog)

set(APPLY_REALSENSE        1)
set(APPLY_OPENCV_CAPTURE   0)

find_package( OpenCV REQUIRED )
#find_package( gflags REQUIRED )


# realsense2
if(APPLY_REALSENSE)
    message("apply intel realsense2")
    #include_directories()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_REALSENSE")
    set(DEPENDENCIES ${DEPENDENCIES} realsense2 gflags ${OpenCV_LIBS})
endif()

if(APPLY_OPENCV_CAPTURE)
    message("apply opencv video capturing")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_OPENCV_CAPTURE")
endif()

message("DEPENDENCIES: ${DEPENDENCIES}")

add_executable(
    ${PN}
    ssd_detect_webcam.cpp
)

target_link_libraries(
    ${PN}
    libcaffe.so
    libpbox.so

    ${DEPENDENCIES}
)
