cmake_minimum_required(VERSION 3.10)
project(MyLib)

add_library(MyLib program_files/Parser.cpp program_files/MyRequest.cpp program_files/WorkWithJson.cpp)

include_directories(/home/sergi/labs/cpr/include)
include_directories(/home/sergi/labs/json/include)

target_link_libraries(MyLib cpr)