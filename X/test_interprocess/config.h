#ifndef test_interprocess_config_h
#define test_interprocess_config_h

#include "Config/config_boost.h"
#include "Config/config_gtest.h"

//#define USER_DEFINED_MAIN 1

#if defined(_WIN64) || defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define BOOST_USE_WINDOWS_H
#endif

#endif // test_interprocess_config_h
