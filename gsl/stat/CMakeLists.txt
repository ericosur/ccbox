cmake_minimum_required(VERSION 3.10)

set(PN "statgsl")
project(${PN})

set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_CXX_STANDARD 11)

#FindGSL()
find_package( GSL REQUIRED )

find_library(
    GSL_LIB
    NAMES gsl
    PATHS /usr/lib /usr/libx86_64-linux-gnu /usr/local/lib /opt/local/lib)

if(GSL_LIB)
    message("gsl: ${GSL_LIB}")
    include_directories(${GSL_INCLUDE_DIRS})
    link_directories(/opt/local/lib)
endif()

#find_package( OpenCV REQUIRED )
#set(DEPENDENCIES realsense2 ${OpenCV_LIBS})


add_executable(
    ${PN}
    main.cpp
    test.cpp
)

target_link_libraries(
    ${PN}
    ${GSL_LIBRARY}
    ${GSL_CBLAS_LIBRARY}
)
