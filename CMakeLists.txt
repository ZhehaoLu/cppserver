cmake_minimum_required(VERSION 4.0)
project(myproj)

set(CMAKE_CXX_STANDARD 23)

add_executable(myproj main.cpp)

add_subdirectory(apps)
