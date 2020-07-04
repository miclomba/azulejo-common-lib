project("test_interprocess")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{GMOCK_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
if (APPLE)
"$ENV{X_LINK_DIR}/Interprocess/Debug"
else()
"$ENV{X_LINK_DIR}/Interprocess"
endif()
"$ENV{GTEST_LINK_DIR}"
"$ENV{GMOCK_LINK_DIR}"
"$ENV{BOOST_LINK_DIR}" 
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
