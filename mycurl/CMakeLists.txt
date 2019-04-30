cmake_minimum_required(VERSION 3.0)

set(PN "mycurl")
project(${PN})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

find_package(CURL REQUIRED)
include_directories(CURL_INCLUDE_DIR)
message("curl include dir: ${CURL_INCLUDE_DIR}")

# json.hpp
#include_directories("$ENV{HOME}/src/github/json/single_include/nlohmann")
include_directories("/usr/local/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_JSON")

add_executable(
    ${PN}
    main.cpp
    qweather.cpp
    curlutil.cpp
)

target_link_libraries(
    ${PN}
    ${CURL_LIBRARIES}
)
