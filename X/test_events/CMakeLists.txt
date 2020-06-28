project("test_events")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"$ENV{X_LINK_DIR}/Entities"
"$ENV{X_LINK_DIR}/Events"
"$ENV{GTEST_LINK_DIR}"
"$ENV{BOOST_LINK_DIR}" 
)

add_executable("${PROJECT_NAME}"
"test_event_channel.cpp" 
"test_event_consumer.cpp" 
"test_event_emitter.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Entities"
"Events" 
gtestd
gtest_maind
)

gtest_discover_tests("${PROJECT_NAME}" DISCOVERY_TIMEOUT 60)
