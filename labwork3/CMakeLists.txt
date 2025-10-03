cmake_minimum_required(VERSION 3.29)

project(labwork3)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)

add_executable(labwork3 main.cpp)

target_link_libraries(labwork3 PUBLIC mylib)