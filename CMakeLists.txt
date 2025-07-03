cmake_minimum_required(VERSION 3.12)

project(X VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "INSTALL_PATH: $ENV{INSTALL_PATH}")
message(STATUS "OMP_INCL_DIR: $ENV{OMP_INCL_DIR}")
message(STATUS "OMP_LINK_DIR: $ENV{OMP_LINK_DIR}")
message(STATUS "VCPKG_INCL_DIR: $ENV{VCPKG_INCL_DIR}")
message(STATUS "VCPKG_LINK_DIR: $ENV{VCPKG_LINK_DIR}")
message(STATUS "X_INCL_DIR: $ENV{X_INCL_DIR}")
message(STATUS "X_LINK_DIR: $ENV{X_LINK_DIR}")
message(STATUS "CMAKE_PREFIX_PATH: $ENV{CMAKE_PREFIX_PATH}")
message(STATUS "PYTHON_VERSION: $ENV{PYTHON_VERSION}")
message(STATUS "BOOST_PYTHON_VERSION: $ENV{BOOST_PYTHON_VERSION}")
message(STATUS "GTEST_LINK_DIR: $ENV{GTEST_LINK_DIR}")
message(STATUS "GTEST_LINK_BIN: $ENV{GTEST_LINK_BIN}")
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

add_subdirectory("src/DatabaseAdapters")
add_subdirectory("src/Entities")
add_subdirectory("src/Events")
add_subdirectory("src/FilesystemAdapters")
add_subdirectory("src/Interprocess")
add_subdirectory("src/Intraprocess")
add_subdirectory("src/Resources")
# add_subdirectory("src/EntitiesModule")
# add_subdirectory("src/ResourcesModule")

# SETUP_GOOGLE_TEST
#Note: make sure to add the following option in CMakeLists.txt of the googletest project
#to avoid linking erros related to static vs dynamic libs:
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(GoogleTest)
enable_testing()

add_subdirectory("src/test_database_adapters")
add_subdirectory("src/test_entities")
add_subdirectory("src/test_events")
add_subdirectory("src/test_filesystem_adapters")
add_subdirectory("src/test_intraprocess")
add_subdirectory("src/test_resources")
# # TODO: cpprest sdk is not portable between windows and mac - replace with Drogon
# add_subdirectory("src/test_interprocess")

#############
## INSTALL ##
#############

# Install the include directory in /usr/local/include
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include)

# Generates and installs a version file for CMake package configuration, 
# which is typically used when someone wants to find_package() your project 
# from another CMake project.

# Includes CMake utilities for generating package config and version files.
include(CMakePackageConfigHelpers)

# Creates a version file for this package (e.g. XConfigVersion.cmake)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Installs the generated version file into your install prefix.
# e.g. /usr/local/share/X/XConfigVersion.cmake
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION share/${PROJECT_NAME}
)
