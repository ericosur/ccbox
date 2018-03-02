cmake_minimum_required(VERSION 3.0)

set(PN "capture")
project(${PN})

set(APPLY_INTEL_OPENCV   0)
set(CMAKE_CXX_FLAGS "-Wall -O3")

set(CMAKE_CXX_STANDARD 11)
set(APPLY_JSON         1)

# if support c++11, turn on json.hpp
# if(COMPILER_SUPPORTS_CXX11)
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# elseif(COMPILER_SUPPORTS_CXX0X)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
# endif()

# json.hpp
if(APPLY_JSON)
    include_directories("$ENV{HOME}/src/github/json/single_include/nlohmann")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_JSON")
endif()

add_executable(
    ${PN}
    main.cpp
    captureDemo.cpp
)

if(APPLY_INTEL_OPENCV)
    message("Will use intel opencv")
    set(Intel_OpenCV "/opt/intel/computer_vision_sdk_2017.1.163/opencv")
    include_directories(${Intel_OpenCV}"/include")
    link_directories(${Intel_OpenCV}"/lib")
    set(Intel_OpenCV_LIBS "-lopencv_dnn -lopencv_ml -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_flann -lopencv_objdetect -lopencv_imgproc -lopencv_core")

    target_link_libraries(
        ${PN}
        ${Intel_OpenCV_LIBS}
        ${OpenCV_LIBS}
    )
else()
    find_package( OpenCV REQUIRED )
    target_link_libraries(
        ${PN}
        ${OpenCV_LIBS}
    )
endif()

