project("Interprocess")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"$ENV{BOOST_LINK_DIR}" 
)

add_library("${PROJECT_NAME}" SHARED
"ISharedMemory.cpp" 
"RESTServer.cpp"
)

target_link_libraries("${PROJECT_NAME}" 
"cpprest_2_10d"
"boost_system$ENV{BOOST_LIB_SUFFIX}"
)

install(TARGETS ${PROJECT_NAME} DESTINATION $ENV{INSTALL_PATH})

