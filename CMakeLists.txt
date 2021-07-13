cmake_minimum_required(VERSION 3.16)
project(DVSimv)

set(CMAKE_CXX_STANDARD 14)

add_executable(dvsim src/DistanceVectorSimulator.cpp)

add_subdirectory(src)