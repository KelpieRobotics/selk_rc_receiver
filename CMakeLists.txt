cmake_minimum_required(VERSION 3.9)
project(selk_rc_transmitter)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

add_subdirectory(raspberry-sbus)

add_library(rc_channels
    selk_rc_msgs/build/cpp/rc_channels.pb.cc
    selk_rc_msgs/build/cpp/rc_channels.pb.h
)

target_link_libraries(rc_channels PUBLIC ${Protobuf_LIBRARIES})
target_include_directories(rc_channels PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(joystick_receiver
    joystick_receiver.cpp
    joystick_receiver.h
)
target_link_libraries(joystick_receiver PUBLIC rc_channels rcdrivers)

add_executable(main main.cpp)
target_link_libraries(main PUBLIC joystick_receiver)
