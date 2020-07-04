project("test_intraprocess")

if (APPLE)
set(INTRAPROCESS_LIB "$ENV{X_LINK_DIR}/Intraprocess/Debug")
else()
set(INTRAPROCESS_LIB "$ENV{X_LINK_DIR}/Intraprocess")
endif()

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"${INTRAPROCESS_LIB}"
"$ENV{GTEST_LINK_DIR}"
"$ENV{BOOST_LINK_DIR}" 
)

add_executable("${PROJECT_NAME}"
"test_iomp_runnable.cpp" 
"test_thread_pool.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Intraprocess" 
gtestd
gtest_maind
)

gtest_discover_tests("${PROJECT_NAME}" DISCOVERY_TIMEOUT 60)
