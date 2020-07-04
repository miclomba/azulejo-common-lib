project("test_entities")

if (APPLE)
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities/Debug")
else()
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
endif()

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"${ENTITIES_LIB}"
"$ENV{GTEST_LINK_DIR}"
"$ENV{BOOST_LINK_DIR}" 
)

add_executable("${PROJECT_NAME}"
"test_entity.cpp" 
"test_entity_hierarchy.cpp" 
"test_entity_registry.cpp" 
"test_istreamable_entity.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Entities"
gtestd
gtest_maind
)

gtest_discover_tests("${PROJECT_NAME}" DISCOVERY_TIMEOUT 60)
