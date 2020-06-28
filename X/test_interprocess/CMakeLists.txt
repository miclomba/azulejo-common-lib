project("test_interprocess")

include_directories(
"$ENV{X_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{GMOCK_INCL_DIR}"
"$ENV{BOOST_DIR}"
)

link_directories(
"$ENV{X_LINK_DIR}/Interprocess"
"$ENV{GTEST_LINK_DIR}"
"$ENV{BOOST_DIR}/stage/lib" 
)

add_executable("${PROJECT_NAME}"
"chat_client_server.cpp" 
"test_async_server.cpp" 
"test_connection_handler.cpp" 
"test_shared_memory.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Interprocess"
gmockd
gmock_maind
gtestd
gtest_maind
)

gtest_discover_tests("${PROJECT_NAME}" DISCOVERY_TIMEOUT 60)
