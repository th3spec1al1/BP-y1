cmake_minimum_required(VERSION 3.12)

project(
    labwork8
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

cmake_policy(SET CMP0079 NEW)

add_subdirectory(bin)

add_subdirectory(lib)

enable_testing()
add_subdirectory(tests)
