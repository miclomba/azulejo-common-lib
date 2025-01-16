#ifndef config_config_gtest_h
#define config_config_gtest_h

#define GTEST_LANG_CXX11
#define GTEST_LINKED_AS_SHARED_LIBRARY 1

#if defined(_WIN64) || defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define ROOT_FILESYSTEM "C:/"
#define TEST_DIRECTORY "/users/miclomba/downloads"
#elif defined(__APPLE__) || defined(__MACH__)
#define ROOT_FILESYSTEM "/"
#define TEST_DIRECTORY "/Users/miclomba/Downloads"
#elif defined(__linux__)
#define ROOT_FILESYSTEM "/"
#define TEST_DIRECTORY "/Users/miclomba/Downloads"
#elif defined(__FreeBSD__)
#define ROOT_FILESYSTEM "/"
#define TEST_DIRECTORY "/Users/miclomba/Downloads"
#elif defined(__ANDROID__)
#define ROOT_FILESYSTEM "/"
#define TEST_DIRECTORY "/Users/miclomba/Downloads"
#endif

#endif
