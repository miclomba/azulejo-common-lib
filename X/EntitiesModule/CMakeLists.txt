project("EntitiesModule")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
"$ENV{PYTHON3_INCL_DIR}"
)

link_directories(
"$ENV{X_LINK_DIR}/Entities"
"$ENV{BOOST_LINK_DIR}"
"$ENV{PYTHON3_LINK_DIR}"
)

add_library("${PROJECT_NAME}" SHARED
"EntitiesModule.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Entities"
)

install(TARGETS ${PROJECT_NAME} DESTINATION $ENV{INSTALL_PATH})

