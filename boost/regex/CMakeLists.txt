cmake_minimum_required(VERSION 3.0)

set(PN "regex")
project(${PN})

find_package( Boost COMPONENTS regex REQUIRED )

include_directories( ${Boost_INCLUDE_DIR} )
link_directories( ${Boost_LIBRARY_DIR} )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

message( "Boost version:" ${Boost_VERSION} )
message( "Boost lib version:" ${Boost_LIB_VERSION} )
message( "Boost libraries: ${Boost_LIBRARIES}")



add_executable(
    ${PN}
    main.cpp
)

target_link_libraries(
    ${PN}
    LINK_PUBLIC
    ${Boost_LIBRARIES}
)
