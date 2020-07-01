project("Intraprocess")

include_directories(
"$ENV{OMP_INCL_DIR}"
)

link_directories(
"$ENV{OMP_LINK_DIR}"
)

add_library("${PROJECT_NAME}" SHARED
"IOMPRunnable.cpp" 
"ThreadPool.cpp" 
)

install(TARGETS ${PROJECT_NAME} DESTINATION $ENV{INSTALL_PATH})

