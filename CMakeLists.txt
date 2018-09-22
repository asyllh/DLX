cmake_minimum_required(VERSION 3.8)
project(DLX)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp func.cpp func.h constants.cpp constants.h)
add_executable(DLX ${SOURCE_FILES})