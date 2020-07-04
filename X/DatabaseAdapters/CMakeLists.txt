project("DatabaseAdapters")

if (APPLE)
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
set(RESOURCES_LIB "$ENV{X_LINK_DIR}/Resources")
else()
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
set(RESOURCES_LIB "$ENV{X_LINK_DIR}/Resources")
endif()

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
"sqlite3.c" 
)

target_link_libraries("${PROJECT_NAME}" 
"Entities"
"Resources" 
)

install(TARGETS ${PROJECT_NAME} DESTINATION $ENV{INSTALL_PATH})

