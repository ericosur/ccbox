cmake_minimum_required(VERSION 3.0)

set(PN "capture")
project(${PN})

set(APPLY_INTEL_OPENCV   0)
set(CMAKE_CXX_FLAGS "-Wall -O3")

set(CMAKE_CXX_STANDARD 11)
set(APPLY_JSON         1)
set(APPLY_REALSENSE    1)

# if support c++11, turn on json.hpp
# if(COMPILER_SUPPORTS_CXX11)
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# elseif(COMPILER_SUPPORTS_CXX0X)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
# endif()

# json.hpp
if(APPLY_JSON)
    message("apply json.hpp")
    include_directories("$ENV{HOME}/src/github/json/single_include/nlohmann")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_JSON")
endif()

# realsense2
if(APPLY_REALSENSE)
    message("apply intel realsense2")
    #include_directories()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_REALSENSE")
endif()

find_package( OpenCV REQUIRED )
set(DEPENDENCIES realsense2 ${OpenCV_LIBS})

add_executable(
    ${PN}
    main.cpp
    captureDemo.cpp
    rs-imshow.cpp
)

target_link_libraries(
    ${PN}
    ${DEPENDENCIES}
)

