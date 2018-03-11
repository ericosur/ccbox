cmake_minimum_required(VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_C_FLAGS "-Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(APPLY_REALSENSE    1)

# Define project name
set(PN "pbox")
project(${PN})

# json.hpp
#include_directories("$ENV{HOME}/src/github/json/single_include/")

# realsense2
if(APPLY_REALSENSE)
    message("apply intel realsense2")
    #include_directories()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_REALSENSE")
    find_package( OpenCV REQUIRED )
    set(DEPENDENCIES realsense2 ${OpenCV_LIBS})
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
        MY_IPC.cpp
    )

# header files for install
file(GLOB LIB_HEADERS
        pbox.h
        MY_IPC.hpp
        UEFIType.h
    )

# Create static library
#add_library(${LIB_NAME} STATIC ${LIB_SOURCES})

# Create shared library
add_library(${LIB_NAME} SHARED ${LIB_SOURCES})

# Install library
install(TARGETS ${LIB_NAME} DESTINATION /usr/local/lib/${PN})

# Install library headers
install(FILES ${LIB_HEADERS} DESTINATION /usr/local/include/${PN})

