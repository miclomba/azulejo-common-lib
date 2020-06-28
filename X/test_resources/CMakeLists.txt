project("test_resources")

include_directories(
"$ENV{X_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{BOOST_DIR}"
)

link_directories(
"$ENV{X_LINK_DIR}/Resources"
"$ENV{GTEST_LINK_DIR}"
"$ENV{BOOST_DIR}/stage/lib" 
)

add_executable("${PROJECT_NAME}"
"test_iresource.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Resources"
gtestd
gtest_maind
)

gtest_discover_tests("${PROJECT_NAME}" DISCOVERY_TIMEOUT 60)
