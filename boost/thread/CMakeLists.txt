cmake_minimum_required(VERSION 2.8)

set(PN "thread")

project(${PN})

find_package( Boost COMPONENTS thread timer REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

# pbox library
include_directories("/usr/local/include/pbox")
link_directories("/usr/local/lib/pbox")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

message( "Boost version:" ${Boost_VERSION} )
message( "Boost lib version:" ${Boost_LIB_VERSION} )

add_executable(
    ${PN}
    main.cpp
    simplenotify.cpp
    func.cpp
)

target_link_libraries(
    ${PN}
    ${Boost_LIBRARIES}
    libpbox.so
)
