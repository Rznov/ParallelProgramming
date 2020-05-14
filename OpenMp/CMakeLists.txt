cmake_minimum_required(VERSION 3.16)
project(OpenMp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_executable(OpenMp main.cpp)