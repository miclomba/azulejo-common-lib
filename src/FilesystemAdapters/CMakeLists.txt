project(FilesystemAdapters VERSION 1.0.0)

set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
set(RESOURCES_LIB "$ENV{X_LINK_DIR}/Resources")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{VCPKG_INCL_DIR}"
)

link_directories(
"${ENTITIES_LIB}"
"${RESOURCES_LIB}"
"$ENV{VCPKG_LINK_DIR}" 
)

add_library(${PROJECT_NAME} SHARED
"EntityDeserializer.cpp" 
"EntitySerializer.cpp" 
"ISerializableEntity.cpp" 
"ISerializableResource.cpp" 
"ResourceDeserializer.cpp" 
"ResourceSerializer.cpp" 
)

target_link_libraries(${PROJECT_NAME}
"Entities"
"Resources"
"boost_filesystem"
"boost_system"
)

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

