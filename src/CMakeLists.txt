cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

add_library(pc_host_lib STATIC
    IpCodeConversion.cpp
    ip4.cpp
    Websocket.cpp
    utils/AtomicWrapper.cpp
    websocket/IWebSocketClient.cpp
    websocket/websocketpp/WebSocketppClient.cpp
)

set_property(TARGET pc_host_lib PROPERTY COMPILE_FLAGS "-Wall -pedantic")
set_source_files_properties(
    websocket/websocketpp/WebSocketppClient.cpp
    websocket/IWebSocketClient.cpp
    PROPERTIES
    COMPILE_FLAGS -std=c++17 # use different C++ version, don't use -Wall
)

target_include_directories(pc_host_lib PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/deps/bit_read_write/include
    ${PROJECT_SOURCE_DIR}/deps/websocketpp
)
