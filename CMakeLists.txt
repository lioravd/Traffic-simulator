cmake_minimum_required(VERSION 3.23)
project(Traffic_simulator C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS -pthread)
add_executable(Traffic_simulator
        Traffic_simulator.c)
