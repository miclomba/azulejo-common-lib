project("FilesystemAdapters")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"$ENV{X_LINK_DIR}/Entities"
"$ENV{X_LINK_DIR}/Resources"
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
