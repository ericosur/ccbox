cmake_minimum_required(VERSION 3.0)

set(PN "capture")
project(${PN})

set(APPLY_INTEL_OPENCV   0)
set(CMAKE_CXX_FLAGS "-Wall -O3")

set(CMAKE_CXX_STANDARD 11)
set(APPLY_JSON         1)
set(APPLY_REALSENSE    1)

# opencv is MUST
find_package( OpenCV REQUIRED )

# if support c++11, turn on json.hpp
# if(COMPILER_SUPPORTS_CXX11)
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# elseif(COMPILER_SUPPORTS_CXX0X)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
# endif()

# json.hpp
if(APPLY_JSON)
    message("apply json.hpp, use json.hpp from github repository")
    include_directories("$ENV{HOME}/src/github/json/single_include/nlohmann")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_JSON")
endif()

# realsense2
if(APPLY_REALSENSE)

    find_library(
        REALSENSE_LIB
        NAMES realsense2
        PATHS /usr/lib /usr/libx86_64-linux-gnu /usr/local/lib)

    if (REALSENSE_LIB)
        message("apply intel realsense2")
        #include_directories()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_REALSENSE")
        set(DEPENDENCIES realsense2 ${OpenCV_LIBS})

        include_directories("$ENV{HOME}/src/github/librealsense/include")
        include_directories("$ENV{HOME}/src/librealsense/include")
    else()
        message("WARNING realsense2 not found, turn off this feature...")
        set(APPLY_REALSENSE    0)
        set(DEPENDENCIES ${OpenCV_LIBS})
    endif()

endif()



#find_package( OpenCV REQUIRED )
#set(DEPENDENCIES realsense2 ${OpenCV_LIBS})


message("====> DEPENDENCIES: ${DEPENDENCIES}")

add_executable(
    ${PN}
    main.cpp
    captureDemo.cpp
    cvutil.cpp
    rs-imshow.cpp
)

target_link_libraries(
    ${PN}
    ${DEPENDENCIES}
)
