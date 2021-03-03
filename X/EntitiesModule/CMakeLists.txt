project("EntitiesModule")

set(PYTHON_VERSION "python39")
set(PYTHON_VERSION_OSX "python37")

if (APPLE)
#make sure to brew install python3
#make sure that $HOME/user-config.jam contains the following before building boost in order to build libboost_python37 instead of libboost_python27:
#using python : : /usr/local/Cellar/python/3.7.8/Frameworks/Python.framework/Versions/3.7/bin/python3 : /usr/local/Cellar/python/3.7.8/Frameworks/Python.framework/Versions/3.7/include/python3.7m ;
set(Python_ROOT_DIR "/usr/local/Cellar/python/3.7.8")
find_package(Python COMPONENTS Development)
endif()

set(ENTITIES_LIB "$ENV{X_LINK_DIR}/Entities")

if (MSVC)
include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
"$ENV{PYTHON3_INCL_DIR}"
)
elseif(APPLE)
include_directories(
"$ENV{X_INCL_DIR}"
"$ENV{BOOST_INCL_DIR}"
"${Python_INCLUDE_DIRS}"
)
endif()

if (MSVC)
link_directories(
"${ENTITIES_LIB}"
"$ENV{BOOST_LINK_DIR}"
"$ENV{PYTHON3_LINK_DIR}"
)
elseif(APPLE)
link_directories(
"${ENTITIES_LIB}"
"$ENV{BOOST_LINK_DIR}"
"${Python_LIBRARY_DIRS}"
)
endif()

add_library("${PROJECT_NAME}" SHARED
"EntitiesModule.cpp" 
)

if (MSVC)
target_link_libraries("${PROJECT_NAME}" 
"Entities"
"boost_${PYTHON_VERSION}$ENV{BOOST_LIB_SUFFIX}"
"${PYTHON_VERSION}"
)
elseif(APPLE)
target_link_libraries("${PROJECT_NAME}" 
"Entities"
"boost_${PYTHON_VERSION_OSX}$ENV{BOOST_LIB_SUFFIX}"
"${Python_LIBRARIES}"
)
endif()

if (MSVC)
install(FILES $ENV{X_LINK_DIR}/${PROJECT_NAME}/${PROJECT_NAME}.dll DESTINATION $ENV{INSTALL_PATH} RENAME ${PROJECT_NAME}.pyd)
elseif(APPLE)
install(FILES $ENV{X_LINK_DIR}/${PROJECT_NAME}/Debug/lib${PROJECT_NAME}.dylib DESTINATION $ENV{INSTALL_PATH} RENAME ${PROJECT_NAME}.pyd)
endif()

