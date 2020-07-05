project("Intraprocess")

if (APPLE)
find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS})
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS})
endif()

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

if (APPLE)
target_link_libraries("${PROJECT_NAME}"
"OpenMP::OpenMP_CXX"
)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION $ENV{INSTALL_PATH})

