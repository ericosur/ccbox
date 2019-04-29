# cmake needs this line
cmake_minimum_required(VERSION 3.0)

# Define project name
set(PN "resize")
project(${PN})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_JSON")

# RECOMMEND:
# source /opt/intel/openvino/bin/setupvars.sh

message("OpenCV_DIR: ${OpenCV_DIR}")
# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories("/usr/local/include")

# libmytool.so
link_directories("/usr/local/lib/mytool")

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
#message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# Declare the executable target built from your sources
add_executable(
    ${PN}
    main.cpp
    test.cpp
)

# Link your application with OpenCV libraries
target_link_libraries(
    ${PN}
    libmytool.so
    ${OpenCV_LIBS}
)
