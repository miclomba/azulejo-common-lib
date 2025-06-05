project(Intraprocess VERSION 1.0.0)

if (APPLE OR UNIX OR MSVC)
#make sure to have brew install libomp
find_package(OpenMP REQUIRED)
endif()

if (APPLE OR UNIX OR MSVC)
include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{OMP_INCL_DIR}"
)

link_directories(
"$ENV{OMP_LINK_DIR}"
)
endif()

add_library(${PROJECT_NAME} SHARED
"IOMPRunnable.cpp" 
"ThreadPool.cpp" 
)

if (APPLE OR UNIX OR MSVC)
target_link_libraries(${PROJECT_NAME}
"OpenMP::OpenMP_CXX"
)
endif()

#############
## INSTALL ##
#############

# This should be PUBLIC or PRIVATE — INTERFACE is invalid for compiled targets
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Install target and associate it with export set
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets    # <-- match this in the EXPORT command
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install the export set (this now works because it matches above)
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION share/${PROJECT_NAME})

