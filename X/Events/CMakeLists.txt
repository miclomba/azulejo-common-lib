project("Events")

include_directories(
"$ENV{X_DIR}"
"$ENV{BOOST_DIR}"
)

link_directories(
"$ENV{X_DIR}/x64/Debug"
"$ENV{BOOST_DIR}/stage/lib" 
)

add_library("${PROJECT_NAME}" SHARED
"EventChannel.cpp" 
"IEventConsumer.cpp" 
"IEventEmitter.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Entities" 
)