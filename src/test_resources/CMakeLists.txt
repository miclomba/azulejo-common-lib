project(test_resources VERSION 1.0.0)

set(RESOURCES_LIB "$ENV{X_LINK_DIR}/Resources")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{VCPKG_INCL_DIR}"
)

link_directories(
"${RESOURCES_LIB}"
"$ENV{GTEST_LINK_DIR}"
"$ENV{GTEST_LINK_BIN}"
"$ENV{VCPKG_LINK_DIR}" 
)

add_executable(${PROJECT_NAME}
"test_iresource.cpp" 
)

target_link_libraries(${PROJECT_NAME}
"Resources"
gtest
gtest_main
)

# gtest_discover_tests(${PROJECT_NAME} DISCOVERY_TIMEOUT 60)
