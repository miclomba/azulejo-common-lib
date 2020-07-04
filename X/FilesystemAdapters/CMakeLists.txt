project("FilesystemAdapters")

if (APPLE)
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities/Debug")
set(RESOURCES_LIB "$ENV{X_LINK_DIR}/Resources/Debug")
else()
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
set(RESOURCES_LIB "$ENV{X_LINK_DIR}/Resources")
endif()

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"${ENTITIES_LIB}"
"${RESOURCES_LIB}"
"$ENV{BOOST_LINK_DIR}" 
)

add_library("${PROJECT_NAME}" SHARED
"EntityDeserializer.cpp" 
"EntitySerializer.cpp" 
"ISerializableEntity.cpp" 
"ISerializableResource.cpp" 
"ResourceDeserializer.cpp" 
"ResourceSerializer.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Entities"
"Resources" 
)

install(TARGETS ${PROJECT_NAME} DESTINATION $ENV{INSTALL_PATH})

