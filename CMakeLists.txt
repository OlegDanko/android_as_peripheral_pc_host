cmake_minimum_required(VERSION 3.5)

project(phonepad_host LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/bit_read_write)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/websocketpp)

add_executable(phonepad_host ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(phonepad_host PRIVATE
    phonepad_host_lib
)

set_property(TARGET phonepad_host PROPERTY COMPILE_FLAGS "-Wall -pedantic")

target_include_directories(phonepad_host PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/bit_read_write/include
)

install(TARGETS phonepad_host
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
