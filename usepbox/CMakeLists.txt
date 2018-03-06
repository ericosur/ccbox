cmake_minimum_required(VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_C_FLAGS "-Wall -O3")
set(APPLY_DEMO1      1)
set(APPLY_DEMO2      1)
set(APPLY_REALSENSE  0)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Define project name
set(PN "foo")
project(${PN})

find_package( OpenCV REQUIRED )

# DEMO1
if(APPLY_DEMO1)
    message("turn on demo1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_DEMO1")
endif()

# DEMO2
if(APPLY_DEMO2)
    message("turn on demo2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_DEMO2")
endif()

# realsense2
if(APPLY_REALSENSE)
    message("apply intel realsense2")
    #include_directories()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_REALSENSE")
    set(DEPENDENCIES realsense2 )
endif()


# pbox library
include_directories("/usr/local/include/pbox")
link_directories("/usr/local/lib/pbox")

add_executable(${PN}
    main.cpp
    getver.cpp
)

target_link_libraries(
    ${PN}
    libpbox.so
    ${DEPENDENCIES}
    ${OpenCV_LIBS}
)
