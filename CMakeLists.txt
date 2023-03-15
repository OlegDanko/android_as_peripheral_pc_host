cmake_minimum_required(VERSION 3.5)

project(pc_host LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(pc_host main.cpp)

install(TARGETS pc_host
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
