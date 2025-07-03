project(test_database_adapters VERSION 1.0.0)

set(DATABASE_ADAPTERS_LIB "$ENV{X_LINK_DIR}/DatabaseAdapters")
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
set(RESOURCES_LIB "$ENV{X_LINK_DIR}/Resources")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{VCPKG_INCL_DIR}"
)

link_directories(
"${DATABASE_ADAPTERS_LIB}"
"${ENTITIES_LIB}"
"${RESOURCES_LIB}"
"$ENV{GTEST_LINK_DIR}"
"$ENV{GTEST_LINK_BIN}"
"$ENV{VCPKG_LINK_DIR}" 
)

add_executable(${PROJECT_NAME}
"test_entity_detabularizer.cpp" 
"test_entity_hierarchy_blob.cpp" 
"test_entity_tabularizer.cpp" 
"test_itabularizable_entity.cpp" 
"test_itabularizable_resource.cpp" 
"test_resource_detabularizer.cpp" 
"test_resource_tabularizer.cpp" 
"test_sqlite.cpp" 
"test_sqlite_blob.cpp" 
)

target_link_libraries(${PROJECT_NAME}
"DatabaseAdapters"
"Entities"
"Resources" 
gtest
gtest_main
"boost_filesystem"
)

# gtest_discover_tests(${PROJECT_NAME} DISCOVERY_TIMEOUT 60)
