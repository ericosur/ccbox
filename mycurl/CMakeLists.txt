cmake_minimum_required(VERSION 2.8)

set(PN "mycurl")

project(${PN})

find_package( CURL REQUIRED )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

# json.hpp
#include_directories("$ENV{HOME}/src/github/json/single_include/nlohmann")
include_directories("/usr/local/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_JSON")

add_executable(
    ${PN}
    mycurl.cpp
)

target_link_libraries(
    ${PN}
    ${CURL_LIBRARIES}
)
