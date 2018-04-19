cmake_minimum_required(VERSION 3.0)

set(PN "cppsend")
project(${PN})

set(CMAKE_CXX_FLAGS "-Wall -O3")

set(CMAKE_CXX_STANDARD 11)
set(APPLY_JSON         1)

# opencv is MUST
find_package( OpenCV REQUIRED )

# json.hpp
if(APPLY_JSON)
    # message("apply json.hpp, use json.hpp from github repository")
    # include_directories("$ENV{HOME}/src/github/json/single_include/nlohmann")
    message("apply json.hpp, use json.hpp from /usr/local/include")
    include_directories("/usr/local/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_JSON")
endif()

link_directories("/usr/local/lib/mytool")

#find_package( OpenCV REQUIRED )
set(DEPENDENCIES ${OpenCV_LIBS})
message("====> DEPENDENCIES: ${DEPENDENCIES}")

add_executable(
    ${PN}
    main.cpp
    readset.cpp
)

target_link_libraries(
    ${PN}
    ${DEPENDENCIES}
    libmytool.so
)
