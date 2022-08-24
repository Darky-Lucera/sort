cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME Sort)
project(${PROJECT_NAME})

# Define Release by default
#--------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    message(STATUS "Build type not specified: Use Release by default")
endif(NOT CMAKE_BUILD_TYPE)

add_executable(radix
    mainRadix.cpp
    radix.h
)