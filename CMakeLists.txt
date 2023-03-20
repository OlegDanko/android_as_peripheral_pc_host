cmake_minimum_required(VERSION 3.5)

project(pc_host LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS_RELEASE "-O0")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/bit_read_write)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/websocketpp)

add_executable(pc_host ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(pc_host PRIVATE
    pc_host_lib
)
set_property(TARGET pc_host PROPERTY COMPILE_FLAGS "-Wall -pedantic")

target_include_directories(pc_host PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/bit_read_write/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/websocketpp
)

install(TARGETS pc_host
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
