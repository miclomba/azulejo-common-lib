project(ResourcesModule VERSION 1.0.0)

if (APPLE)
#make sure to brew install python3
#make sure that $HOME/user-config.jam contains the following before building boost in order to build libboost_python37 instead of libboost_python27:
#using python : : /usr/local/Cellar/python/3.7.8/Frameworks/Python.framework/Versions/3.7/bin/python3 : /usr/local/Cellar/python/3.7.8/Frameworks/Python.framework/Versions/3.7/include/python3.7m ;
set(Python_ROOT_DIR "$ENV{Python_ROOT_DIR}")
find_package(Python COMPONENTS Development)
endif()

set(RESOURCES_LIB "$ENV{X_LINK_DIR}/Resources")

include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
"$ENV{PYTHON3_INCL_DIR}"
)

link_directories(
"${RESOURCES_LIB}"
"$ENV{BOOST_LINK_DIR}"
"$ENV{PYTHON3_LINK_DIR}"
)

add_library("${PROJECT_NAME}" SHARED
"ResourcesModule.cpp" 
)

target_link_libraries("${PROJECT_NAME}" 
"Resources"
"boost_$ENV{BOOST_PYTHON_VERSION}$ENV{BOOST_LIB_SUFFIX}"
"$ENV{PYTHON_VERSION}"
)

#############
## INSTALL ##
#############

# if (MSVC)
# install(FILES $ENV{X_LINK_DIR}/${PROJECT_NAME}/${PROJECT_NAME}.dll DESTINATION $ENV{INSTALL_PATH} RENAME ${PROJECT_NAME}.pyd)
# elseif(APPLE)
# install(FILES $ENV{X_LINK_DIR}/${PROJECT_NAME}/Debug/lib${PROJECT_NAME}.dylib DESTINATION $ENV{INSTALL_PATH} RENAME ${PROJECT_NAME}.pyd)
# endif()
