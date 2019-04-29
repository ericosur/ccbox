cmake_minimum_required(VERSION 3.0)

set(PN "simplecap")
project(${PN})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")


# use local build of opencv, intel opencv lacks needed modules
message("OpenCV_DIR: ${OpenCV_DIR}")
find_package( OpenCV REQUIRED )
message("opencv ver: ${OpenCV_VERSION}")

include_directories("/usr/local/include")
include_directories(${OpenCV_INCLUDE_DIRS})

# build each programs
add_executable(
    ${PN}
    main.cpp
)
target_link_libraries(
    ${PN}
    ${OpenCV_LIBS}
)

add_executable(
    fe2d fe2d.cpp sett.cpp sett.h
)
target_link_libraries(
    fe2d ${OpenCV_LIBS}
)

add_executable(
    surfdemo surfdemo.cpp sett.cpp sett.h
)
target_link_libraries(
    surfdemo ${OpenCV_LIBS}
)


add_executable(
    latch latch.cpp  sett.cpp sett.h
)
target_link_libraries(
    latch ${OpenCV_LIBS}
)

add_executable(
    surf surf.cpp  sett.cpp sett.h
)
target_link_libraries(
    surf ${OpenCV_LIBS}
)

add_executable(
    surflann surflann.cpp  sett.cpp sett.h
)
target_link_libraries(
    surflann ${OpenCV_LIBS}
)


add_executable(
    vec vec.cpp
)
target_link_libraries(
    vec ${OpenCV_LIBS}
)

add_executable(fcd findContours_demo.cpp)
target_link_libraries(fcd ${OpenCV_LIBS})
