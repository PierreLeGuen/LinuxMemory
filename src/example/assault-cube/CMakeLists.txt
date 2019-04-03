cmake_minimum_required(VERSION 3.13)
project(AC_external_hack)

set(CMAKE_CXX_STANDARD 14)

include_directories(LinuxMemoryAPI/include)

add_executable(load_AC_external_hack
        main.cpp)

target_link_libraries(load_AC_external_hack PUBLIC ${CMAKE_SOURCE_DIR}/LinuxMemoryAPI/lib/libLinuxMemory.so)
