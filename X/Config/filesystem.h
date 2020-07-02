#ifndef config_config_filesystem_h
#define config_config_filesystem_h

#if defined(__APPLE__) || defined(__MACH__)
#include <boost/filesystem.hpp>
#define Path boost::filesystem::path
#else
#include <filesystem>
#define Path std::filesystem::path
#endif

#endif