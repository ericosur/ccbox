cmake_minimum_required(VERSION 3.0)

set(PN "rectdraw")
project(${PN})

find_package( OpenCV REQUIRED )
message("opencv ver: ${OpenCV_VERSION}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

add_executable(
    ${PN}
    rectdraw.cpp
)

target_link_libraries(
    ${PN}
    ${OpenCV_LIBS}
)
