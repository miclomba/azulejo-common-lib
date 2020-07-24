project("test_interprocess")

set(INTERPROCESS_LIB "$ENV{X_LINK_DIR}/Interprocess")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"${INTERPROCESS_LIB}"
"$ENV{GTEST_LINK_DIR}"
"$ENV{GTEST_LINK_BIN}"
"$ENV{BOOST_LINK_DIR}" 
)

add_executable("${PROJECT_NAME}"
"chat_client_server.cpp" 
"main.cpp"
"rest_client_server.cpp" 
"test_async_server.cpp" 
"test_connection_handler.cpp" 
"test_shared_memory.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Interprocess"
gmockd
gtestd
gtest_maind
gmock_maind
"boost_chrono$ENV{BOOST_LIB_SUFFIX}"
"boost_system$ENV{BOOST_LIB_SUFFIX}"
"boost_thread$ENV{BOOST_LIB_SUFFIX}"
)

if (MSVC)
gtest_discover_tests("${PROJECT_NAME}" DISCOVERY_TIMEOUT 60)
endif()
