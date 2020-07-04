project("test_resources")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
if (APPLE)
"$ENV{X_LINK_DIR}/Resources/Debug"
else()
"$ENV{X_LINK_DIR}/Resources"
endif()
"$ENV{GTEST_LINK_DIR}"
"$ENV{BOOST_LINK_DIR}" 
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
