project("test_database_adapters")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
if (APPLE)
"$ENV{X_LINK_DIR}/DatabaseAdapters/Debug"
"$ENV{X_LINK_DIR}/Entities/Debug"
"$ENV{X_LINK_DIR}/Resources/Debug"
else()
"$ENV{X_LINK_DIR}/DatabaseAdapters"
"$ENV{X_LINK_DIR}/Entities"
"$ENV{X_LINK_DIR}/Resources"
endif()
"$ENV{GTEST_LINK_DIR}"
"$ENV{BOOST_LINK_DIR}" 
)

add_executable("${PROJECT_NAME}"
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

target_link_libraries("${PROJECT_NAME}" 
"DatabaseAdapters"
"Entities"
"Resources" 
gtestd
gtest_maind
)

gtest_discover_tests("${PROJECT_NAME}" DISCOVERY_TIMEOUT 60)
