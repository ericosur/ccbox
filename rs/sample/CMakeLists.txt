cmake_minimum_required(VERSION 3.0)

set(PN  "rs_sample")

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_C_FLAGS "-Wall -O3")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function -Wno-sign-compare -Wno-unused-result")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPU_ONLY=1 -DUSE_OPENCV -DUSE_MKL")


# pbox library
#include_directories("/usr/local/include/pbox")
#include_directories("/usr/local/include")
#link_directories("/usr/local/lib/pbox")


find_package( OpenCV REQUIRED )
set(DEPENDENCIES ${OpenCV_LIBS})

set(APPLY_REALSENSE        1)

# realsense2
if(APPLY_REALSENSE)

    find_library(
        REALSENSE_LIB
        NAMES realsense2
        PATHS /usr/local/lib /usr/libx86_64-linux-gnu /usr/lib  )

    if (REALSENSE_LIB)
        message("=====> apply intel realsense2")
        #include_directories()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_REALSENSE")
        set(DEPENDENCIES ${DEPENDENCIES} realsense2 gflags ${OpenCV_LIBS})

        set(librealsense_dir "$ENV{HOME}/src/github/librealsense/build")
        if ( IS_DIRECTORY librealsense_dir )
            link_directories(librealsense_dir)
            message("dir: ${librealsense_dir}")
        endif()

        set(librealsense_dir "$ENV{HOME}/src/librealsense/build")
        if ( IS_DIRECTORY librealsense_dir )
            link_directories(librealsense_dir)
            message("dir: ${librealsense_dir}")
        endif()

        #link_directories(${REALSENSE_LIB})

    else()
        message("realsense2 library not found, disable related features")

    endif()

endif()


message("DEPENDENCIES: ${DEPENDENCIES}")

add_executable(
    ir
    ir.cpp
    dist.cpp
)

target_link_libraries(
    ir
    ${DEPENDENCIES}
)

add_executable(
    bgr
    bgr.cpp
)

target_link_libraries(
    bgr
    ${DEPENDENCIES}
)
