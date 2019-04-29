cmake_minimum_required(VERSION 3.0)

set(PN "qrcode")
project(${PN})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")
if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

# intel opencv lacks needed modules
message("OpenCV_DIR: ${OpenCV_DIR}")
find_package( OpenCV REQUIRED )
message("opencv ver: ${OpenCV_VERSION}")
message("opencv inc: ${OpenCV_INCLUDE_DIRS}")
include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(${PN} qrcode.cpp)
target_link_libraries(${PN} ${OpenCV_LIBS})
