cmake_minimum_required(VERSION "3.12")

project("X")

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
set(CMAKE_CXX_FLAGS "/permissive- /EHsc /WX /openmp")
add_compile_definitions(_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS)
add_definitions(-D_WIN32_WINNT=0x0A00)
endif()

add_subdirectory("DatabaseAdapters")
add_subdirectory("Entities")
add_subdirectory("EntitiesModule")
add_subdirectory("Events")
add_subdirectory("FilesystemAdapters")
add_subdirectory("Interprocess")
add_subdirectory("Intraprocess")
add_subdirectory("Resources")
add_subdirectory("ResourcesModule")

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
add_subdirectory("test_intraprocess")
add_subdirectory("test_resources")

