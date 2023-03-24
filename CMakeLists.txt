cmake_minimum_required(VERSION 3.5)

project(pc_host LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/bit_read_write)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/websocketpp)

find_package(X11 REQUIRED)
find_library(XTST_LIB NAMES Xtst)
find_library(XINPUT_LIB NAMES Xi)
find_library(EVDEV_LIB NAMES evdev)

message(${X11_LIBRARIES})
message(${XTST_LIB})
message(${XINPUT_LIB})
message(${EVDEV_LIB})

add_executable(pc_host ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(pc_host PRIVATE
    pc_host_lib
    X11
    ${XTST_LIB}
    ${XINPUT_LIB}
    ${EVDEV_LIB}
)
set_property(TARGET pc_host PROPERTY COMPILE_FLAGS "-Wall -pedantic")

target_include_directories(pc_host PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/bit_read_write/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/websocketpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/libevdev
)

install(TARGETS pc_host
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
