cmake_minimum_required(VERSION "3.12")

project("X")

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
set(CMAKE_CXX_FLAGS "/permissive- /EHsc")
endif()

add_subdirectory("DatabaseAdapters")
add_subdirectory("Entities")
add_subdirectory("Events")
add_subdirectory("FilesystemAdapters")
add_subdirectory("Interprocess")
add_subdirectory("Resources")
if (NOT APPLE)
add_subdirectory("EntitiesModule")
add_subdirectory("Intraprocess")
add_subdirectory("ResourcesModule")
endif()

# SETUP_GOOGLE_TEST
#Note: make sure to add the following option in CMakeLists.txt of the googletest project
#to avoid linking erros related to static vs dynamic libs:
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(GoogleTest)
enable_testing()

add_subdirectory("test_database_adapters")
add_subdirectory("test_entities")
add_subdirectory("test_events")
add_subdirectory("test_filesystem_adapters")
add_subdirectory("test_interprocess")
add_subdirectory("test_resources")
if (NOT APPLE)
add_subdirectory("test_intraprocess")
endif()

