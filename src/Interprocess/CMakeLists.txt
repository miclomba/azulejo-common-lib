project(Interprocess VERSION 1.0.0)

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"$ENV{BOOST_LINK_DIR}" 
)

add_library(${PROJECT_NAME} SHARED
"ISharedMemory.cpp" 
)

if (MSVC)
target_link_libraries(${PROJECT_NAME}
"cpprest_2_10"
"boost_system$ENV{BOOST_LIB_SUFFIX}"
)
elseif(APPLE)
target_link_libraries(${PROJECT_NAME}
"cpprest"
"boost_system$ENV{BOOST_LIB_SUFFIX}"
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

