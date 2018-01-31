cmake_minimum_required(VERSION 2.8)

set(PN "first")

project(${PN})

find_package( Boost REQUIRED )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

message( "Boost version:" ${Boost_VERSION} )
message( "Boost lib version:" ${Boost_LIB_VERSION} )

add_executable(
    ${PN}
    first.cpp
)

target_link_libraries(
    ${PN}
    ${Boost_LIBRARIES}
)
