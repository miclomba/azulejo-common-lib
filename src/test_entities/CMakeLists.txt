project(test_entities VERSION 1.0.0)

set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{VCPKG_INCL_DIR}"
)

link_directories(
"${ENTITIES_LIB}"
"$ENV{GTEST_LINK_DIR}"
"$ENV{GTEST_LINK_BIN}"
"$ENV{VCPKG_LINK_DIR}" 
)

add_executable(${PROJECT_NAME}
"test_entity.cpp" 
"test_entity_hierarchy.cpp" 
"test_entity_registry.cpp" 
"test_istreamable_entity.cpp" 
)

target_link_libraries(${PROJECT_NAME}
"Entities"
gtest
gtest_main
"boost_filesystem"
)

# gtest_discover_tests(${PROJECT_NAME} DISCOVERY_TIMEOUT 60)
