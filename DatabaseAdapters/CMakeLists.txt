project("DatabaseAdapters")

set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
set(RESOURCES_LIB "$ENV{X_LINK_DIR}/Resources")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"${ENTITIES_LIB}"
"${RESOURCES_LIB}"
"$ENV{BOOST_LINK_DIR}" 
)

add_library("${PROJECT_NAME}" SHARED
"EntityDetabularizer.cpp" 
"EntityHierarchyBlob.cpp" 
"EntityTabularizer.cpp" 
"ITabularizableEntity.cpp" 
"ITabularizableResource.cpp" 
"ResourceDetabularizer.cpp" 
"ResourceTabularizer.cpp" 
"Sqlite.cpp" 
"SqliteBlob.cpp" 
 
)

target_link_libraries("${PROJECT_NAME}" 
"Entities"
"Resources" 
"boost_filesystem$ENV{BOOST_LIB_SUFFIX}"
"sqlite3"
)

install(TARGETS ${PROJECT_NAME} DESTINATION $ENV{INSTALL_PATH})

