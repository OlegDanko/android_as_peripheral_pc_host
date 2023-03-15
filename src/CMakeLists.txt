cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(pc_host_lib STATIC LibA.cpp)
target_include_directories(pc_host_lib PRIVATE ${PROJECT_SOURCE_DIR}/include)
