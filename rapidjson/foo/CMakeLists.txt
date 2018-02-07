cmake_minimum_required(VERSION 2.8)

set(PN "foo")

project(${PN})

#find_package( OpenCV REQUIRED )

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

include_directories("$ENV{HOME}/src/github/rapidjson/include/")

# pbox library
include_directories("/usr/local/include")
link_directories("/usr/local/lib/pbox")

add_executable(
    ${PN}
    main.cpp
    foo.cpp
)

target_link_libraries(
    ${PN}
    libpbox.a
)
