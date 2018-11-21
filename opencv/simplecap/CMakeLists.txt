cmake_minimum_required(VERSION 3.0)

set(PN "simplecap")
project(${PN})

set(OpenCV_DIR    "/usr/local/share/OpenCV")
find_package( OpenCV REQUIRED )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -O3")

include_directories("/usr/local/include")

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

