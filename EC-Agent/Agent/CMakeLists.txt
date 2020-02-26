cmake_minimum_required(VERSION 3.14)
project(Agent)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS -pthread)

add_executable(Agent Handler.cpp Handler.h Server.cpp Server.h main.cpp)