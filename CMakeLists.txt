cmake_minimum_required(VERSION 3.13)

project(main C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")

file(GLOB SRC_FILES src/*.c)
include_directories(inc)

add_executable(${PROJECT_NAME} ${SRC_FILES})