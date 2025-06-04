#ifndef interprocess_config_h
#define interprocess_config_h

#include "Config/config_boost.h"

#if defined(_WIN64) || defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define INTERPROCESS_DLL_EXPORT __declspec(dllexport)
#elif defined(__APPLE__) || defined(__MACH__)
#define INTERPROCESS_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__linux__)
#define INTERPROCESS_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__FreeBSD__)
#define INTERPROCESS_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__ANDROID__)
#define INTERPROCESS_DLL_EXPORT __attribute__((visibility ("default")))
#endif

#if defined(_WIN64) || defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define BOOST_USE_WINDOWS_H
#endif

#endif // interprocess_config_h
