project("Events")

if (APPLE)
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
else()
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
endif()

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"${ENTITIES_LIB}"
"$ENV{BOOST_LINK_DIR}" 
)

add_library("${PROJECT_NAME}" SHARED
"EventChannel.cpp" 
"IEventConsumer.cpp" 
"IEventEmitter.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Entities" 
)

install(TARGETS ${PROJECT_NAME} DESTINATION $ENV{INSTALL_PATH})
