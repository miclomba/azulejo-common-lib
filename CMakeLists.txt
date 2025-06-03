cmake_minimum_required(VERSION "3.12")

project("X")

set(CMAKE_CXX_STANDARD 17)

message(STATUS "INSTALL_PATH: $ENV{INSTALL_PATH}")
message(STATUS "OMP_INCL_DIR: $ENV{OMP_INCL_DIR}")
message(STATUS "OMP_LINK_DIR: $ENV{OMP_LINK_DIR}")
message(STATUS "BOOST_INCL_DIR: $ENV{BOOST_INCL_DIR}")
message(STATUS "BOOST_LINK_DIR: $ENV{BOOST_LINK_DIR}")
message(STATUS "X_INCL_DIR: $ENV{X_INCL_DIR}")
message(STATUS "X_LINK_DIR: $ENV{X_LINK_DIR}")
message(STATUS "CMAKE_PREFIX_PATH: $ENV{CMAKE_PREFIX_PATH}")
message(STATUS "PYTHON_VERSION: $ENV{PYTHON_VERSION}")
message(STATUS "BOOST_PYTHON_VERSION: $ENV{BOOST_PYTHON_VERSION}")
message(STATUS "GTEST_LINK_DIR: $ENV{GTEST_LINK_DIR}")
message(STATUS "GTEST_LINK_BIN: $ENV{GTEST_LINK_BIN}")
message(STATUS "GTEST_INCL_DIR: $ENV{GTEST_INCL_DIR}")
message(STATUS "PYTHON3_LINK_DIR: $ENV{PYTHON3_LINK_DIR}")
message(STATUS "PYTHON3_INCL_DIR: $ENV{PYTHON3_INCL_DIR}")
message(STATUS "CXX_COMPILER: $ENV{CXX_COMPILER}")
message(STATUS "C_COMPILER: $ENV{C_COMPILER}")

if (MSVC)
set(CMAKE_CXX_FLAGS "/permissive- /EHsc /WX /openmp")
add_compile_definitions(_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS)
add_definitions(-D_WIN32_WINNT=0x0A00)
elseif (APPLE)
set(CMAKE_C_COMPILER "$ENV{C_COMPILER}" CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER "$ENV{CXX_COMPILER}" CACHE STRING "" FORCE)
elseif (UNIX)
set(CMAKE_CXX_FLAGS "-fopenmp")
endif()

add_subdirectory("DatabaseAdapters")
add_subdirectory("Entities")
add_subdirectory("Events")
add_subdirectory("FilesystemAdapters")
add_subdirectory("Interprocess")
add_subdirectory("Intraprocess")
add_subdirectory("Resources")
if (MSVC)
add_subdirectory("EntitiesModule")
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
add_subdirectory("test_intraprocess")
add_subdirectory("test_resources")
# TODO: cpprest sdk is not portable between windows and mac - replace with Drogon
if (MSVC)
add_subdirectory("test_interprocess")
endif()

