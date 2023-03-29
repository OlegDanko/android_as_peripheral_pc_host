cmake_minimum_required(VERSION 3.5)

project(phonepad_host_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB TEST_SRC
    Test_ip4.cpp
    TestIpAddrStringCodeCvt.cpp
    websocket/TestRemote.cpp
    websocket/websocketpp/server/TestWebSocketppServer.cpp
)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
add_executable(phonepad_host_tests main.cpp ${TEST_SRC})
target_link_libraries(phonepad_host_tests Boost::unit_test_framework phonepad_host_lib)

target_include_directories(phonepad_host_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(phonepad_host_tests PRIVATE ${CMAKE_SOURCE_DIR}/include/)
target_include_directories(phonepad_host_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/deps/FakeIt/single_header/boost)
