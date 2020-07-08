cmake_minimum_required(VERSION 3.16)
project(Dijkstra)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_executable(Dijkstra main.cpp Dijkstra.cpp Graph.cpp)