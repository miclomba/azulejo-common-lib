project(test_intraprocess VERSION 1.0.0)

set(INTRAPROCESS_LIB "$ENV{X_LINK_DIR}/Intraprocess")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{VCPKG_INCL_DIR}"
)

link_directories(
"${INTRAPROCESS_LIB}"
"$ENV{GTEST_LINK_DIR}"
"$ENV{GTEST_LINK_BIN}"
"$ENV{VCPKG_LINK_DIR}" 
)

add_executable(${PROJECT_NAME}
"test_iomp_runnable.cpp" 
"test_thread_pool.cpp" 
)

target_link_libraries(${PROJECT_NAME} 
"Intraprocess" 
gtest
gtest_main
)

# gtest_discover_tests(${PROJECT_NAME} DISCOVERY_TIMEOUT 60)
