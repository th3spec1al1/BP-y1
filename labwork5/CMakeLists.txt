cmake_minimum_required(VERSION 3.5.0)

project(labwork5-SergioK06 VERSION 0.1.0 LANGUAGES C CXX)

SET(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)

add_executable(labwork5-SergioK06 main.cpp)

target_link_libraries(labwork5-SergioK06 PUBLIC mylib)
