cmake_minimum_required(VERSION "3.12")

project("X")

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("Events")
add_subdirectory("Interprocess")
add_subdirectory("Intraprocess")
add_subdirectory("Resources")

# SETUP_GOOGLE_TEST
#Note: make sure to add the following option in CMakeLists.txt of the googletest project
#to avoid linking erros related to static vs dynamic libs:
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(GoogleTest)
enable_testing()

add_subdirectory("test_events")
add_subdirectory("test_interprocess")
add_subdirectory("test_intraprocess")
add_subdirectory("test_resources")

