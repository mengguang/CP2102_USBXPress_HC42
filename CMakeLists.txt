cmake_minimum_required(VERSION 3.14)
project(CP2102_USBXPress_HC42)

set(CMAKE_CXX_STANDARD 14)

link_directories(lib)
include_directories(lib)
add_executable(CP2102_USBXPress_HC42 main.cpp lib/SiUSBXp.h)
target_link_libraries(CP2102_USBXPress_HC42 SiUSBXp)