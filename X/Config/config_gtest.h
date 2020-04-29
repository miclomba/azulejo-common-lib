#ifndef config_config_gtest_h
#define config_config_gtest_h

#define GTEST_LANG_CXX11 1

#ifdef _WIN32
#define ROOT_FILESYSTEM "C:/"
#define TEST_DIRECTORY "/users/miclomba/downloads"
#else
#define ROOT_FILESYSTEM "/"
#define TEST_DIRECTORY "/home/miclomba/downloads"
#endif

#endif
