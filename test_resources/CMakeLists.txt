project("test_resources")

set(RESOURCES_LIB "$ENV{X_LINK_DIR}/Resources")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"${RESOURCES_LIB}"
"$ENV{GTEST_LINK_DIR}"
"$ENV{GTEST_LINK_BIN}"
"$ENV{BOOST_LINK_DIR}" 
)

add_executable("${PROJECT_NAME}"
"test_iresource.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Resources"
gtest
gtest_main
)

gtest_discover_tests("${PROJECT_NAME}" DISCOVERY_TIMEOUT 60)
