cmake_minimum_required(VERSION 3.0)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_C_FLAGS "-Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")

# -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(APPLY_REALSENSE    0)

# Define project name
set(PN "pbox")
project(${PN})

# realsense2
if(APPLY_REALSENSE)

    find_library(
        REALSENSE_LIB
        NAMES realsense2
        PATHS /usr/lib /usr/libx86_64-linux-gnu /usr/local/lib)

    if (REALSENSE_LIB)
        message("apply intel realsense2")
        #include_directories()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_REALSENSE")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_OPENCV")
        find_package( OpenCV REQUIRED )
        set(DEPENDENCIES realsense2 ${OpenCV_LIBS})

        include_directories("$ENV{HOME}/src/github/librealsense/include")
        include_directories("$ENV{HOME}/src/librealsense/include")
    else()
        set(APPLY_REALSENSE    0)
    endif()

endif()

################################################################
set(LIB_NAME ${PN})

# source files
file(GLOB LIB_SOURCES
        pbox.cpp
        hsv2rgb.cpp
        ptz.cpp
        util.cpp
        rs-imshow.cpp
    )

# header files for install
file(GLOB LIB_HEADERS
        pbox.h
    )

include_directories("/usr/local/include")

# Create static library
#add_library(${LIB_NAME} STATIC ${LIB_SOURCES})

# Create shared library
add_library(${LIB_NAME} SHARED ${LIB_SOURCES})

# Install library
install(TARGETS ${LIB_NAME} DESTINATION /usr/local/lib/${PN})

# Install library headers
install(FILES ${LIB_HEADERS} DESTINATION /usr/local/include/${PN})

