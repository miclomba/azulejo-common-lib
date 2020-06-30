#ifndef entity_config_h
#define entity_config_h

#include "Config/config_boost.h"

#if defined(_WIN64) || defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define ENTITY_DLL_EXPORT __declspec(dllexport)
#elif defined(__APPLE__) || defined(__MACH__)
#define ENTITY_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__linux__)
#define ENTITY_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__FreeBSD__)
#define ENTITY_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__ANDROID__)
#define ENTITY_DLL_EXPORT __attribute__((visibility ("default")))
#endif

#endif // entity_config_h
