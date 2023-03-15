cmake_minimum_required(VERSION 3.5)

project(pc_host LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)

add_executable(pc_host ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(pc_host PRIVATE pc_host_lib)

install(TARGETS pc_host
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
