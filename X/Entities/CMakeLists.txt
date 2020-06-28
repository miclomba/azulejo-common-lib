project("Entities")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"$ENV{BOOST_LINK_DIR}" 
)

add_library("${PROJECT_NAME}" SHARED
"Entity.cpp" 
"EntityHierarchy.cpp" 
"IStreamableEntity.cpp" 
)
