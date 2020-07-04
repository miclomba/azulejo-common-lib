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

target_link_libraries("${PROJECT_NAME}" 
"boost_filesystem$ENV{BOOST_LIB_SUFFIX}"
)

install(TARGETS ${PROJECT_NAME} DESTINATION $ENV{INSTALL_PATH})

