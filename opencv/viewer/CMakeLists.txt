cmake_minimum_required(VERSION 3.0)

set(PN "viewer")
project(${PN})

set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")

set(CMAKE_CXX_STANDARD 11)
set(APPLY_JSON         1)

# RECOMMEND:
# source /opt/intel/openvino/bin/setupvars.sh
message("OpenCV_DIR: ${OpenCV_DIR}")
find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})

# json.hpp
if(APPLY_JSON)
    # message("apply json.hpp, use json.hpp from github repository")
    # include_directories("$ENV{HOME}/src/github/json/single_include/nlohmann")
    message("apply json.hpp, use json.hpp from /usr/local/include")
    include_directories("/usr/local/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_JSON")
endif()

set(DEPENDENCIES ${OpenCV_LIBS})
#message("====> DEPENDENCIES: ${DEPENDENCIES}")

add_executable(
    ${PN}
    cvui.h
    main.cpp
)

target_link_libraries(
    ${PN}
    ${DEPENDENCIES}
)
