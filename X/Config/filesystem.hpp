#ifndef config_config_filesystem_hpp
#define config_config_filesystem_hpp

#if defined(__APPLE__) || defined(__MACH__)
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#endif