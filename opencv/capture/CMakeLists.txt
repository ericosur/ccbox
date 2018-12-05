cmake_minimum_required(VERSION 3.0)

set(PN "capture")
project(${PN})

set(APPLY_INTEL_OPENCV   0)
set(CMAKE_CXX_FLAGS "-Wall -O3")
# if support openmp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wno-unused-function")

set(CMAKE_CXX_STANDARD 11)
set(APPLY_JSON         1)
set(APPLY_REALSENSE    1)

# opencv is MUST
#set(OpenCV_DIR    "/opt/intel/opencv/share/OpenCV")
set(OpenCV_DIR    "/usr/local/share/OpenCV")
find_package( OpenCV REQUIRED )
message("opencv ver: ${OpenCV_VERSION}")

# if support c++11, turn on json.hpp
# if(COMPILER_SUPPORTS_CXX11)
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# elseif(COMPILER_SUPPORTS_CXX0X)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
# endif()

# json.hpp
if(APPLY_JSON)
    # message("apply json.hpp, use json.hpp from github repository")
    # include_directories("$ENV{HOME}/src/github/json/single_include/nlohmann")
    message("apply json.hpp, use json.hpp from /usr/local/include")
    include_directories("/usr/local/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_JSON")
endif()

set(DEPENDENCIES ${OpenCV_LIBS})

# realsense2
if(APPLY_REALSENSE)

    find_library(
        REALSENSE_LIB
        NAMES  realsense2
        PATHS  /usr/local/lib /usr/lib /usr/lib/x86_64-linux-gnu)

    if (REALSENSE_LIB)
        message("apply intel realsense2")
        #include_directories()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_REALSENSE")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder")
        set(DEPENDENCIES "realsense2;${DEPENDENCIES}")

        include_directories("$ENV{HOME}/src/github/librealsense/include")
        include_directories("$ENV{HOME}/src/librealsense/include")

        message("rs: ${REALSENSE_LIB}")
    else()
        message("WARNING realsense2 not found, turn off this feature...")
        set(APPLY_REALSENSE    0)
    endif()

endif()


#message("====> DEPENDENCIES: ${DEPENDENCIES}")

if(1)
add_executable(
    ${PN}
    cvui.h
    cvutil.cpp cvutil.h
    rsutil.cpp rsutil.h
    miscutil.cpp miscutil.h
    readsetting.cpp readsetting.h
    captureDemo.cpp
    rs-imshow.cpp
    main.cpp
)

target_link_libraries(
    ${PN}
    ${DEPENDENCIES}
)
endif()

if(0)
add_executable(
    hell a.cpp rsutil.cpp rsutil.h
)
target_link_libraries(
    hell ${DEPENDENCIES}
)
endif()
