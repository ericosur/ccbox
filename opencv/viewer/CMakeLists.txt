cmake_minimum_required(VERSION 3.0)

set(PN "viewer")
project(${PN})

set(APPLY_INTEL_OPENCV   0)
set(CMAKE_CXX_FLAGS "-Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")

set(CMAKE_CXX_STANDARD 11)
set(APPLY_JSON         1)

# opencv is MUST
set(OpenCV_DIR    "/opt/intel/opencv/share/OpenCV")
#set(OpenCV_DIR    "/usr/local/share/OpenCV")
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
    # message("apply json.hpp, use json.hpp from github repository")
    # include_directories("$ENV{HOME}/src/github/json/single_include/nlohmann")
    message("apply json.hpp, use json.hpp from /usr/local/include")
    include_directories("/usr/local/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_JSON")
endif()

set(DEPENDENCIES ${OpenCV_LIBS})

message("====> DEPENDENCIES: ${DEPENDENCIES}")

add_executable(
    ${PN}
    cvui.h
    main.cpp
)

target_link_libraries(
    ${PN}
    ${DEPENDENCIES}
)
