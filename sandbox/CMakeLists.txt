cmake_minimum_required(VERSION 3.5)

find_package(OpenCV REQUIRED)

find_path(AVCODEC_INCLUDE_DIR NAMES libavcodec/avcodec.h libavutil/avutil.h)
find_library(AVCODEC_LIBRARY NAMES avcodec)
find_library(AVUTIL_LIBRARY NAMES avutil)

message("util lib: ${AVUTIL_LIBRARY}")

file(GLOB DIRS_SANDBOX
    ${AVCODEC_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/deps/libdatachannel/include
    ${PROJECT_SOURCE_DIR}/deps/bit_read_write/include
    ${OpenCV_INCLUDE_DIRS}
)

set(LIBS_SANDBOX
    ${AVCODEC_LIBRARY}
    /usr/lib/libavutil.so
    ${PROJECT_SOURCE_DIR}/deps/x264/build/lib/libx264.so
    ${OpenCV_LIBRARIES}
    datachannel
)
message("all libs: ${LIBS_SANDBOX}")

#add_executable(phonepad_host_sandbox
#    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/Codec.cpp
#)

file(GLOB SRC_SANDBOX
    ${CMAKE_CURRENT_SOURCE_DIR}/Codec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenCVDisplay.cpp
)

add_executable(sandbox_send
    ${CMAKE_CURRENT_SOURCE_DIR}/send_main.cpp
    ${SRC_SANDBOX}
)
add_executable(sandbox_recv
    ${CMAKE_CURRENT_SOURCE_DIR}/recv_main.cpp
    ${SRC_SANDBOX}
)

#target_include_directories(phonepad_host_sandbox PRIVATE
#    ${PROJECT_SOURCE_DIR}/deps/libdatachannel
#    ${PROJECT_SOURCE_DIR}/deps/bit_read_write/include
#)
target_include_directories(sandbox_send PRIVATE ${DIRS_SANDBOX})
target_include_directories(sandbox_recv PRIVATE ${DIRS_SANDBOX})

#target_link_libraries(phonepad_host_sandbox PRIVATE ${LIBS_SANDBOX})
target_link_libraries(sandbox_send PRIVATE ${LIBS_SANDBOX})
target_link_libraries(sandbox_recv PRIVATE ${LIBS_SANDBOX})
