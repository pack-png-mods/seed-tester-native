cmake_minimum_required(VERSION 3.15)
project(seed_tester_native)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(seed_tester_native main.cpp random.cpp generator.cpp)