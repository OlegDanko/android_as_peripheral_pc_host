cmake_minimum_required(VERSION 3.5)

project(pc_host_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
add_executable(pc_host_tests main.cpp Test_ip4.cpp TestIpAddrStringCodeCvt.cpp)
target_link_libraries(pc_host_tests Boost::unit_test_framework pc_host_lib)
target_include_directories(pc_host_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
