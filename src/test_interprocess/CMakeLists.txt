project(test_interprocess VERSION 1.0.0)

set(INTERPROCESS_LIB "$ENV{X_LINK_DIR}/Interprocess")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{VCPKG_INCL_DIR}"
)

link_directories(
"${INTERPROCESS_LIB}"
"$ENV{GTEST_LINK_DIR}"
"$ENV{GTEST_LINK_BIN}"
"$ENV{VCPKG_LINK_DIR}" 
)

add_executable("${PROJECT_NAME}"
"main.cpp"
"main_rest.cpp" 
"main_tcp.cpp" 
"test_connection_handler.cpp" 
"test_rest_client.cpp" 
"test_rest_server.cpp" 
"test_shared_memory.cpp" 
"test_tcp_server.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Interprocess"
gmock
gtest
gtest_main
gmock_main
"boost_chrono"
"boost_filesystem"
"boost_system"
"boost_thread"
)

# gtest_discover_tests("${PROJECT_NAME}" DISCOVERY_TIMEOUT 60)

#############
## INSTALL ##
#############

# if (MSVC)
# install(FILES $ENV{X_LINK_DIR}/${PROJECT_NAME}/${PROJECT_NAME}.exe DESTINATION $ENV{INSTALL_PATH} RENAME rest_server.exe)
# endif()
