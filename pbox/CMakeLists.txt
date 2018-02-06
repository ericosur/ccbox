cmake_minimum_required(VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_C_FLAGS "-Wall -O3")
set(CMAKE_CXX_FLAGS "-Wno-unused-function")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Define project name
set(PN "pbox")
project(${PN})

# json.hpp
include_directories("$ENV{HOME}/src/github/json/src/")

################################################################
set(LIB_NAME ${PN})

# source files
file(GLOB LIB_SOURCES
        pbox.cpp
        hsv2rgb.cpp
    )
# header files for install
file(GLOB LIB_HEADERS
        pbox.h
    )

# Create static library
add_library(${LIB_NAME} STATIC ${LIB_SOURCES})

# Create shared library
#add_library(${LIB_NAME} SHARED ${LIB_SOURCES})

# Install library
install(TARGETS ${LIB_NAME} DESTINATION /usr/local/lib/${PN})

# Install library headers
install(FILES ${LIB_HEADERS} DESTINATION /usr/local/include/${PN})
