#ifndef events_config_h
#define events_config_h

#include "Config/config_boost.h"

#if defined(_WIN64) || defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define EVENTS_DLL_EXPORT __declspec(dllexport)
#elif defined(__APPLE__) || defined(__MACH__)
#define EVENTS_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__linux__)
#define EVENTS_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__FreeBSD__)
#define EVENTS_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__ANDROID__)
#define EVENTS_DLL_EXPORT __attribute__((visibility ("default")))
#endif

#endif
