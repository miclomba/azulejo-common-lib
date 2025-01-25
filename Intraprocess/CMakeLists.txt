project("Intraprocess")

if (APPLE)
#make sure to have brew install libomp
find_package(OpenMP REQUIRED)
endif()

if (APPLE)
include_directories(
"$ENV{OMP_INCL_DIR}"
)

link_directories(
"$ENV{OMP_LINK_DIR}"
)
endif()

add_library("${PROJECT_NAME}" SHARED
"IOMPRunnable.cpp" 
"ThreadPool.cpp" 
)

if (APPLE)
target_link_libraries("${PROJECT_NAME}"
"OpenMP::OpenMP_CXX"
)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION $ENV{INSTALL_PATH})

