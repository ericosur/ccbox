cmake_minimum_required(VERSION 2.8)

set(PN "sem_test")
project(${PN})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

include_directories("$ENV{HOME}/src/github/rapidjson/include/")

# pbox library
include_directories("/usr/local/include")
link_directories("/usr/local/lib/pbox")

add_executable(
    proc1
    proc1.cpp
    semaphore.cpp
)

add_executable(
    proc2
    proc2.cpp
    semaphore.cpp
)

target_link_libraries(
    proc1
    libpbox.so
)

target_link_libraries(
    proc2
    libpbox.so
)
