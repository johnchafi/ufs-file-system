#cmake_minimum_required(VERSION 3.17)
#project(SE)
#
#set(CMAKE_CXX_STANDARD 14)
#
#add_executable(SE main.cpp)

cmake_minimum_required(VERSION 3.17)
project(CodeTP3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(.)

#add_executable(TP3
#        block.cpp
#        disqueVirtuel.cpp
#        main.cpp)

add_executable(CodeTP3
        block.cpp
        block.h
        disqueVirtuel.cpp
        disqueVirtuel.h
        main.cpp)