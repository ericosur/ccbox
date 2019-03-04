cmake_minimum_required(VERSION 3.0)

set(CMAKE_VERBOSE_MAKEFILE   OFF)
set(CMAKE_CXX_STANDARD       11)
set(CMAKE_CXX_FLAGS          "-Wall -O3")
set(CMAKE_C_FLAGS            "-Wall -O3")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Define project name
set(PN "epoch")
project(${PN})

find_package( OpenCV REQUIRED )

# mytool library
include_directories("/usr/local/include/mytool")
link_directories("/usr/local/lib/mytool")

add_executable(${PN}
    main.cpp
)

target_link_libraries(
    ${PN}
    libmytool.so
)
