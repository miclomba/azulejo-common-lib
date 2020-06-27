project("Intraprocess")

add_library("${PROJECT_NAME}" SHARED
"IOMPRunnable.cpp" 
"ThreadPool.cpp" 
)
