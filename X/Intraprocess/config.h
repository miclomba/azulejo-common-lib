#ifndef intraprocess_config_h
#define intraprocess_config_h

#if defined(_WIN64) || defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define INTRAPROCESS_DLL_EXPORT __declspec(dllexport)
#elif defined(__APPLE__) || defined(__MACH__)
#define INTRAPROCESS_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__linux__)
#define INTRAPROCESS_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__FreeBSD__)
#define INTRAPROCESS_DLL_EXPORT __attribute__((visibility ("default")))
#elif defined(__ANDROID__)
#define INTRAPROCESS_DLL_EXPORT __attribute__((visibility ("default")))
#endif

#endif // intraprocess_config_h
