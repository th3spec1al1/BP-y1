cmake_minimum_required(VERSION 3.10)
project(labwork6-SergioK06)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)
include_directories(/home/sergi/labs/cpr/include)
include_directories(/home/sergi/labs/json/include)

add_executable(labwork6-SergioK06 main.cpp)

target_link_libraries(labwork6-SergioK06 MyLib)