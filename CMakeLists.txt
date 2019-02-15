cmake_minimum_required(VERSION 3.13)
project(LinuxMemory C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(LinuxMemory
        LinuxMemory.c
        LinuxMemory.h
        main.c)
