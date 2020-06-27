cmake_minimum_required(VERSION "3.12")

project("X")

# SETUP_GOOGLE_TEST
#Note: make sure to add the following option in CMakeLists.txt of the googletest project
#to avoid linking erros related to static vs dynamic libs:
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
include(GoogleTest)
enable_testing()
# END SETUP_GOOGLE_TEST

set(CMAKE_CXX_STANDARD 17)

message(STATUS "ENV{PATH}: $ENV{PATH}")
message(STATUS "ENV{X_LINK_DIR}: $ENV{X_LINK_DIR}")

add_subdirectory("Events")
add_subdirectory("Intraprocess")
add_subdirectory("test_events")
add_subdirectory("test_intraprocess")

