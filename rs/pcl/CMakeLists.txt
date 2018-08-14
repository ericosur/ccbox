cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(cloud_viewer)

find_package(PCL 1.2 REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

list(REMOVE_ITEM PCL_LIBRARIES "vtkproj4")
add_executable (cloud_viewer cloud_viewer.cpp)
add_executable(read_ply read_ply.cpp)

target_link_libraries (cloud_viewer ${PCL_LIBRARIES})
target_link_libraries (read_ply ${PCL_LIBRARIES})
