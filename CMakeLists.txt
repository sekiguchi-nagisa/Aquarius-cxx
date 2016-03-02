cmake_minimum_required(VERSION 2.7)

project(Aquarius-cxx)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)

enable_testing()
add_subdirectory(test)
