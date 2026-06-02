cmake_minimum_required(VERSION 4.0)
project(Project_Portf_C__)

set(CMAKE_CXX_STANDARD 20)

add_executable(Project_Portf_C__ main.cpp
        LogParser.cpp
        LogParser.h
        DataTypes.h
)

add_executable(Gener_test generator.cpp)
