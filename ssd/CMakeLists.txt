cmake_minimum_required(VERSION 3.0)

set(PN  "ssd_detect")

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_C_FLAGS "-Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function -Wno-sign-compare")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPU_ONLY=1 -DUSE_OPENCV -DUSE_MKL")

set(CAFFE_ROOT  "$ENV{CAFFE_ROOT}")
message("CAFFE_ROOT: ${CAFFE_ROOT}")

# pbox library
include_directories("/usr/local/include/pbox")
include_directories("/usr/local/include")
link_directories("/usr/local/lib/pbox")

set(testdir $ENV{CAFFE_ROOT})
if(IS_DIRECTORY ${testdir})
    set(caffe_root ${testdir})
    message("use caffe_root: ${caffe_root}")
else()
    set(caffe_root $ENV{HOME}/src/github/intel/caffe)
    message("use home local: ${caffe_root}")
endif()

# caffe library
include_directories("${caffe_root}/build/install/include")
message("${caffe_root}/build/install/include")

link_directories("${caffe_root}/build/install/lib")
message("${caffe_root}/build/install/lib")

# mkl
include_directories(${caffe_root}/external/mkl/mklml_lnx_2018.0.1.20171227/include)
link_directories(${caffe_root}/external//mkldnn/install/lib)
link_directories(${caffe_root}/external/mkl/mklml_lnx_2018.0.1.20171227/lib)
message(${caffe_root}/external/mkldnn/install/lib)

find_package( OpenCV REQUIRED )
set(DEPENDENCIES libboost_system.so gflags glog ${OpenCV_LIBS})

set(APPLY_REALSENSE        1)
set(APPLY_MYIPC            1)
set(APPLY_WEBCAM           1)



# realsense2
if(APPLY_REALSENSE)

    find_library(
        REALSENSE_LIB
        NAMES realsense2
        PATHS /usr/lib /usr/libx86_64-linux-gnu /usr/local/lib)

    if (REALSENSE_LIB)
        message("=====> apply intel realsense2")
        #include_directories()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_REALSENSE")
        set(DEPENDENCIES ${DEPENDENCIES} realsense2 gflags ${OpenCV_LIBS})

        set(librealsense_dir "$ENV{HOME}/src/github/librealsense/build")
        if ( IS_DIRECTORY librealsense_dir )
            link_directories(librealsense_dir)
        endif()

        set(librealsense_dir "$ENV{HOME}/src/librealsense/build")
        if ( IS_DIRECTORY librealsense_dir )
            link_directories(librealsense_dir)
        endif()
    else()
        message("realsense2 library not found, disable related features")

    endif()

endif()


if(APPLY_WEBCAM)
    message("=====> apply opencv video capturing")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_OPENCV_CAPTURE")
endif()


if(APPLY_MYIPC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_MYIPC")
    set(EXTRA_SOURCE    "MY_IPC.cpp")
endif()


message("DEPENDENCIES: ${DEPENDENCIES}")

add_executable(
    ${PN}
    ssd_detect_webcam.cpp
    cvutil.cpp
    ssd_setting.cpp
    ssdutil.cpp
    shmutil.cpp
    ${EXTRA_SOURCE}
)

target_link_libraries(
    ${PN}
    libcaffe.so
    libpbox.so

    ${DEPENDENCIES}
)
