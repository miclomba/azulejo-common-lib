project("EntitiesModule")

if (APPLE)
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
else()
set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")
endif()

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
"$ENV{PYTHON3_INCL_DIR}"
)

link_directories(
"${ENTITIES_LIB}"
"$ENV{BOOST_LINK_DIR}"
"$ENV{PYTHON3_LINK_DIR}"
)

add_library("${PROJECT_NAME}" SHARED
"EntitiesModule.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Entities"
)

install(FILES $ENV{X_LINK_DIR}/${PROJECT_NAME}/${PROJECT_NAME}.dll DESTINATION $ENV{INSTALL_PATH} RENAME ${PROJECT_NAME}.pyd)

