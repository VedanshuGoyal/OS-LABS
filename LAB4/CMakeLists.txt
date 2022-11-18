cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

project(CPU Scheduling)
add_executable(generator generator.cpp)
add_executable(process process.cpp)
add_executable(scheduler_2 scheduler_2.cpp)