project("ResourcesModule")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
"$ENV{PYTHON3_INCL_DIR}"
)

link_directories(
"$ENV{X_LINK_DIR}/Resources"
"$ENV{BOOST_LINK_DIR}"
"$ENV{PYTHON3_LINK_DIR}"
)

add_library("${PROJECT_NAME}" SHARED
"ResourcesModule.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Resources"
)

install(FILES $ENV{X_LINK_DIR}/${PROJECT_NAME}/${PROJECT_NAME}.dll DESTINATION $ENV{INSTALL_PATH} RENAME ${PROJECT_NAME}.pyd)
