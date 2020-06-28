project("Entities")

include_directories(
"$ENV{X_DIR}"
"$ENV{BOOST_DIR}"
)

link_directories(
"$ENV{BOOST_DIR}/stage/lib" 
)

add_library("${PROJECT_NAME}" SHARED
"Entity.cpp" 
"EntityHierarchy.cpp" 
"IStreamableEntity.cpp" 
)
