project("test_events")

if (APPLE)
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
set(EVENTS_LIB "$ENV{X_LINK_DIR}/Events")
else()
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
set(EVENTS_LIB "$ENV{X_LINK_DIR}/Events")
endif()

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{GTEST_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"${ENTITIES_LIB}"
"${EVENTS_LIB}"
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
