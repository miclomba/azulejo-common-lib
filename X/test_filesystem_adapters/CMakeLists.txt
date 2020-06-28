project("test_filesystem_adapters")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"$ENV{X_LINK_DIR}/DatabaseAdapters"
"$ENV{X_LINK_DIR}/Entities"
"$ENV{X_LINK_DIR}/FilesystemAdapters"
"$ENV{X_LINK_DIR}/Resources"
"$ENV{GTEST_LINK_DIR}"
"$ENV{BOOST_LINK_DIR}" 
)

add_executable("${PROJECT_NAME}"
"test_entity_deserializer.cpp" 
"test_entity_serializer.cpp" 
"test_iserializable_entity.cpp" 
"test_iserializable_resource.cpp" 
"test_iserializable_resource_2d.cpp" 
"test_resource_deserializer.cpp" 
"test_resource_serializer.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"DatabaseAdapters"
"Entities"
"FilesystemAdapters"
"Resources" 
gtestd
gtest_maind
)

gtest_discover_tests("${PROJECT_NAME}" DISCOVERY_TIMEOUT 60)
