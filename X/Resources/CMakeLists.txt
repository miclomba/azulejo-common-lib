project("Resources")

include_directories(
"$ENV{X_DIR}"
"$ENV{BOOST_DIR}"
)

link_directories(
"$ENV{BOOST_DIR}/stage/lib" 
)

add_library("${PROJECT_NAME}" SHARED
"IResource.cpp" 
)
