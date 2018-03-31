cmake_minimum_required(VERSION 3.0)

set(PN  "myqrc")

set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_C_FLAGS "-Wall -O3")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function -Wno-sign-compare")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPU_ONLY=1 -DUSE_OPENCV -DUSE_MKL")


# pbox library
include_directories("/usr/local/include/pbox")
include_directories("/usr/local/include")
link_directories("/usr/local/lib/pbox")

# quirc
include_directories("/usr/local/include")
link_directories("/usr/local/lib")

find_package( OpenCV REQUIRED )
set(DEPENDENCIES ${OpenCV_LIBS})

message("DEPENDENCIES: ${DEPENDENCIES}")

add_executable(
    ${PN}
    main.cpp
    test.cpp
)

target_link_libraries(
    ${PN}
    libpbox.so
    libquirc.so
    ${DEPENDENCIES}
)
