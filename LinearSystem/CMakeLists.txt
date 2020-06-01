cmake_minimum_required(VERSION 3.16)
project(LinearSystem)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER /Users/arozanov/mpi/bin/mpic++)

include_directories(/Users/arozanov/mpi/bin)

add_executable(LinearSystem main.cpp Matrix.cpp JacobiSolver.cpp)