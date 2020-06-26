cmake_minimum_required(VERSION "3.12") 

project("Events")

set(CMAKE_CXX_STANDARD 17)

include_directories(
"$ENV{X_INCL_DIR}"
"C:/Program Files/boost/boost_1_69_0"
)

link_directories(
"$ENV{X_LINK_DIR}"
"C:/Program Files/boost/boost_1_69_0/stage/lib" 
)

add_library("${PROJECT_NAME}" SHARED
"EventChannel.cpp" 
"IEventConsumer.cpp" 
"IEventEmitter.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Entities" 
)