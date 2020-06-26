cmake_minimum_required(VERSION "3.12") 

project("X")

set(CMAKE_CXX_STANDARD 17)

message(STATUS "ENV{PATH}: $ENV{PATH}")
message(STATUS "ENV{X_LINK_DIR}: $ENV{X_LINK_DIR}")

add_subdirectory("Events")

