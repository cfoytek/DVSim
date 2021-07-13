cmake_minimum_required(VERSION 3.16)
project(DVSimv2)

set(CMAKE_CXX_STANDARD 14)

add_executable(dvsim DistanceVectorSimulator.cpp)

add_subdirectory(src)