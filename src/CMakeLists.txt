cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC_20
    IpCodeConversion.cpp
    ip4.cpp
    websocket/Remote.cpp
    utils/AtomicWrapper.cpp
)
file(GLOB SRC_17
    websocket/IWebSocketClient.cpp
    websocket/websocketpp/WebSocketppClient.cpp
    websocket/websocketpp/server/GenWSPPServer.cpp
)

set_source_files_properties(
    ${SRC_20}
    PROPERTIES
    COMPILE_FLAGS -std=c++20 # use different C++ version, don't use -Wall
)
set_source_files_properties(
    ${SRC_17}
    PROPERTIES
    COMPILE_FLAGS -std=c++17 # use different C++ version, don't use -Wall
)

add_library(phonepad_host_lib STATIC
    ${SRC_20}
    ${SRC_17}
)
set_property(TARGET phonepad_host_lib PROPERTY COMPILE_FLAGS "-Wall -pedantic")

target_include_directories(phonepad_host_lib PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/deps/bit_read_write/include
    ${PROJECT_SOURCE_DIR}/deps/websocketpp
)
