project(DatabaseAdapters VERSION 1.0.0)

set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
set(RESOURCES_LIB "$ENV{X_LINK_DIR}/Resources")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"${ENTITIES_LIB}"
"${RESOURCES_LIB}"
"$ENV{BOOST_LINK_DIR}" 
)

# Define the actual TARGET
add_library(${PROJECT_NAME} SHARED
"EntityDetabularizer.cpp" 
"EntityHierarchyBlob.cpp" 
"EntityTabularizer.cpp" 
"ITabularizableEntity.cpp" 
"ITabularizableResource.cpp" 
"ResourceDetabularizer.cpp" 
"ResourceTabularizer.cpp" 
"Sqlite.cpp" 
"SqliteBlob.cpp" 
)

target_link_libraries(${PROJECT_NAME}
"Entities"
"Resources" 
"boost_filesystem$ENV{BOOST_LIB_SUFFIX}"
"sqlite3"
)

#############
## INSTALL ##
#############

# Sets the include path for your target during build and install time.
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Install target and associate it with export set
# Installs your compiled target (${PROJECT_NAME}).
# ARCHIVE: for static .a or .lib files.
# LIBRARY: for shared .so/.dylib/.dll libraries.
# RUNTIME: for Windows .exe or .dll binaries.
# EXPORT ${PROJECT_NAME}Targets: prepares an export set that defines how this target can be used from other projects.
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Installs the export set to make it usable with find_package(${PROJECT_NAME}) 
# This now works because it matches above
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION share/${PROJECT_NAME})