cmake_minimum_required(VERSION 3.13)
project(LinuxMemory C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/LinuxMemory/lib)
# file(REMOVE ${CMAKE_SOURCE_DIR}/LinuxMemory/include/LinuxMemory.h)

add_library(LinuxMemory SHARED LinuxMemory.c LinuxMemory.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/LinuxMemory.h ${CMAKE_SOURCE_DIR}/LinuxMemory/include COPYONLY)

# file(COPY LinuxMemory.h DESTINATION ${CMAKE_SOURCE_DIR}/LinuxMemory/include)
