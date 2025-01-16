project("test_filesystem_adapters")

set(DATABASE_ADAPTERS_LIB "$ENV{X_LINK_DIR}/DatabaseAdapters")
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
set(FILESYSTEM_ADAPTERS_LIB "$ENV{X_LINK_DIR}/FilesystemAdapters")
set(RESOURCES_LIB "$ENV{X_LINK_DIR}/Resources")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"${DATABASE_ADAPTERS_LIB}"
"${ENTITIES_LIB}"
"${FILESYSTEM_ADAPTERS}"
"${RESOURCES_LIB}"
"$ENV{GTEST_LINK_DIR}"
"$ENV{GTEST_LINK_BIN}"
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
gtest
gtest_main
"boost_filesystem$ENV{BOOST_LIB_SUFFIX}"
)

if (MSVC)
gtest_discover_tests("${PROJECT_NAME}" DISCOVERY_TIMEOUT 60)
endif()
