cmake_minimum_required(VERSION 3.16)
project(ParallelQuicksort)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER /Users/arozanov/mpi/bin/mpic++)
include_directories(/Users/arozanov/mpi/bin)

add_executable(ParallelQuicksort main.cpp IntArray.cpp Quicksort.cpp ProcessGroup.cpp)