project("DatabaseAdapters")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
)

link_directories(
"$ENV{X_LINK_DIR}/Entities"
"$ENV{X_LINK_DIR}/Resources"
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
"sqlite3.c" 
)

target_link_libraries("${PROJECT_NAME}" 
"Entities"
"Resources" 
)

install(TARGETS ${PROJECT_NAME} DESTINATION $ENV{INSTALL_PATH})

