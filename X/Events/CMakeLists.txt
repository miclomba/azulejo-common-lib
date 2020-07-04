project("Events")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
if (APPLE)
"$ENV{X_LINK_DIR}/Entities/Debug"
else()
"$ENV{X_LINK_DIR}/Entities"
endif()
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
